//------------------------------------------------------- ----------------------
// File: Object.h
//-----------------------------------------------------------------------------

#pragma once

#include "Mesh.h"
#include "Camera.h"

#define DIR_FORWARD					0x01
#define DIR_BACKWARD				0x02
#define DIR_LEFT					0x04
#define DIR_RIGHT					0x08
#define DIR_UP						0x10
#define DIR_DOWN					0x20

class CShader;
class CStandardShader;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
#define RESOURCE_TEXTURE2D			0x01
#define RESOURCE_TEXTURE2D_ARRAY	0x02	//[]
#define RESOURCE_TEXTURE2DARRAY		0x03
#define RESOURCE_TEXTURE_CUBE		0x04
#define RESOURCE_BUFFER				0x05

struct SRVROOTARGUMENTINFO
{
	int								m_nRootParameterIndex = 0;
	D3D12_GPU_DESCRIPTOR_HANDLE		m_d3dSrvGpuDescriptorHandle;
};


struct CB_PARTICLE_INFO
{
	UINT							m_iParticleTime;
};
class CTexture
{
public:
	CTexture(int nTextureResources = 1, UINT nResourceType = RESOURCE_TEXTURE2D, int nSamplers = 0);
	virtual ~CTexture();

private:
	int								m_nReferences = 0;

	UINT							m_nTextureType = RESOURCE_TEXTURE2D;

	int								m_nTextures = 0;
	ID3D12Resource					**m_ppd3dTextures = NULL;
	ID3D12Resource					**m_ppd3dTextureUploadBuffers;

	int								m_nSamplers = 0;
	D3D12_GPU_DESCRIPTOR_HANDLE		*m_pd3dSamplerGpuDescriptorHandles = NULL;

public:
	SRVROOTARGUMENTINFO				*m_pRootArgumentInfos = NULL;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	void SetRootArgument(int nIndex, UINT nRootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dsrvGpuDescriptorHandle);
	void SetSampler(int nIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dSamplerGpuDescriptorHandle);

	void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	void UpdateShaderVariable(ID3D12GraphicsCommandList *pd3dCommandList, int nIndex);
	void ReleaseShaderVariables();

	void LoadTextureFromFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, wchar_t *pszFileName, UINT nIndex, bool bIsDDSFile=true);

	int GetTextures() { return(m_nTextures); }
	ID3D12Resource *GetTexture(int nIndex) { return(m_ppd3dTextures[nIndex]); }
	UINT GetTextureType() { return(m_nTextureType); }

	void ReleaseUploadBuffers();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
#define MATERIAL_ALBEDO_MAP			0x01
#define MATERIAL_SPECULAR_MAP		0x02
#define MATERIAL_NORMAL_MAP			0x04
#define MATERIAL_METALLIC_MAP		0x08
#define MATERIAL_EMISSION_MAP		0x10
#define MATERIAL_DETAIL_ALBEDO_MAP	0x20
#define MATERIAL_DETAIL_NORMAL_MAP	0x40

class CGameObject;
class CParticleBillboardObject;

class CMaterial
{
public:
	CMaterial(int nTextures);
	virtual ~CMaterial();

private:
	int								m_nReferences = 0;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

public:
	CShader							*m_pShader = NULL;

	XMFLOAT4						m_xmf4AlbedoColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	XMFLOAT4						m_xmf4EmissiveColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4						m_xmf4SpecularColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4						m_xmf4AmbientColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

	void SetShader(CShader *pShader);
	void SetMaterialType(UINT nType) { m_nType |= nType; }
	void SetTexture(CTexture *pTexture, UINT nTexture = 0);

	virtual void UpdateShaderVariable(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void UpdateShaderVariable(ID3D12GraphicsCommandList *pd3dCommandList,int matID);
	virtual void ReleaseUploadBuffers();

public:
	UINT							m_nType = 0x00;

	float							m_fGlossiness = 0.0f;
	float							m_fSmoothness = 0.0f;
	float							m_fSpecularHighlight = 0.0f;
	float							m_fMetallic = 0.0f;
	float							m_fGlossyReflection = 0.0f;

public:
	int 							m_nTextures = 0;
	_TCHAR							(*m_ppstrTextureNames)[64] = NULL;
	CTexture						**m_ppTextures = NULL; //0:Albedo, 1:Specular, 2:Metallic, 3:Normal, 4:Emission, 5:DetailAlbedo, 6:DetailNormal

	void LoadTextureFromFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, UINT nType, UINT nRootParameter, _TCHAR *pwstrTextureName, CTexture **ppTexture, CGameObject *pParent, FILE *pInFile, CShader *pShader);

public:
	static CShader					*m_pStandardShader;
	static CShader					*m_pSkinnedAnimationShader;

	static void CMaterial::PrepareShaders(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature);

	void SetStandardShader() { CMaterial::SetShader(m_pStandardShader); }
	void SetSkinnedAnimationShader() { CMaterial::SetShader(m_pSkinnedAnimationShader); }
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
#define ANIMATION_TYPE_ONCE			0
#define ANIMATION_TYPE_LOOP			1
#define ANIMATION_TYPE_PINGPONG		2

#define ANIMATION_CALLBACK_EPSILON	0.015f

struct CALLBACKKEY
{
   float  							m_fTime = 0.0f;
   void  							*m_pCallbackData = NULL;
};

struct KEYFRAME
{
   float  							m_fTime = 0.0f;
   float  							m_fValue = 0.0f;
};

class CAnimationCurve
{
public:
	CAnimationCurve() { }
	~CAnimationCurve() { }

public:
   int  							m_nKeyFrames = 0;
   KEYFRAME							*m_pKeyFrames = NULL;
};

//#define _WITH_ANIMATION_SRT
#define _WITH_ANIMATION_INTERPOLATION

class CAnimationCallbackHandler
{
public:
	CAnimationCallbackHandler() { }
	~CAnimationCallbackHandler() { }

public:
   virtual void HandleCallback(void *pCallbackData) { }
};

class CAnimationSet
{
public:
	CAnimationSet();
	~CAnimationSet();

public:
	char							m_pstrName[64];

	float							m_fSpeed = 1.0f;
	float							m_fLength = 0.0f;
	int								m_nFramesPerSecond = 0; //m_fTicksPerSecond

	int								m_nKeyFrameTransforms = 0;
	float							*m_pfKeyFrameTransformTimes = NULL;
	XMFLOAT4X4						**m_ppxmf4x4KeyFrameTransforms = NULL;

#ifdef _WITH_ANIMATION_SRT
	int								m_nKeyFrameScales = 0;
	float							*m_pfKeyFrameScaleTimes = NULL;
	XMFLOAT3						**m_ppxmf3KeyFrameScales = NULL;
	int								m_nKeyFrameRotations = 0;
	float							*m_pfKeyFrameRotationTimes = NULL;
	XMFLOAT4						**m_ppxmf4KeyFrameRotations = NULL;
	int								m_nKeyFrameTranslations = 0;
	float							*m_pfKeyFrameTranslationTimes = NULL;
	XMFLOAT3						**m_ppxmf3KeyFrameTranslations = NULL;
#endif

	float 							m_fPosition = 0.0f;
    int 							m_nType = ANIMATION_TYPE_LOOP; //Once, Loop, PingPong

	int								m_nCurrentKey = -1;

	int 							m_nCallbackKeys = 0;
	CALLBACKKEY 					*m_pCallbackKeys = NULL;

	CAnimationCallbackHandler 		*m_pAnimationCallbackHandler = NULL;

public:
	void SetPosition(float& fTrackPosition,float& oncePosition);
	

	XMFLOAT4X4 GetSRT(int nFrame);

	void SetCallbackKeys(int nCallbackKeys);
	void SetCallbackKey(int nKeyIndex, float fTime, void *pData);
	void SetAnimationCallbackHandler(CAnimationCallbackHandler *pCallbackHandler);

	void *GetCallbackData();
};

class CAnimationSets
{
private:
	int								m_nReferences = 0;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

public:
	CAnimationSets();
	~CAnimationSets();

public:
	int								m_nAnimationFrames = 0; 
	CGameObject						**m_ppAnimationFrameCaches = NULL;

	int								m_nAnimationSets = 0;
	CAnimationSet					*m_pAnimationSets = NULL;

public:
	void SetCallbackKeys(int nAnimationSet, int nCallbackKeys);
	void SetCallbackKey(int nAnimationSet, int nKeyIndex, float fTime, void *pData);
	void SetAnimationCallbackHandler(int nAnimationSet, CAnimationCallbackHandler *pCallbackHandler);
};

class CAnimationTrack
{
public:
	CAnimationTrack() { }
	~CAnimationTrack() { }

public:
    BOOL 							m_bEnable = true;
    float 							m_fSpeed = 1.0f;
    float 							m_fPosition = 0.0f;
    float 							m_fWeight = 1.0f;

    CAnimationSet 					*m_pAnimationSet = NULL;

public:
	void SetAnimationSet(CAnimationSet *pAnimationSet) { m_pAnimationSet = pAnimationSet; }
	void SetEnable(bool bEnable) { m_bEnable = bEnable; }
	void SetSpeed(float fSpeed) { m_fSpeed = fSpeed; }
	void SetWeight(float fWeight) { m_fWeight = fWeight; }
	void SetPosition(float fPosition) { m_fPosition = fPosition; }
};

class CAnimationController 
{
public:
	CAnimationController(int nAnimationTracks, CAnimationSets *pAnimationSets);
	~CAnimationController();

public:
    float 							m_fTime = 0.0f;

	CAnimationSets					*m_pAnimationSets = NULL;

    int 							m_nAnimationTracks = 0;
    CAnimationTrack 				*m_pAnimationTracks = NULL;

public:
	void SetAnimationSets(CAnimationSets *pAnimationSets);

	void SetTrackAnimationSet(int nAnimationTrack, int nAnimationSet);
	void SetTrackEnable(int nAnimationTrack, bool bEnable);
	void SetTrackPosition(int nAnimationTrack, float fPosition);
	void SetTrackSpeed(int nAnimationTrack, float fSpeed);
	void SetTrackWeight(int nAnimationTrack, float fWeight);

	void SetCallbackKeys(int nAnimationSet, int nCallbackKeys);
	void SetCallbackKey(int nAnimationSet, int nKeyIndex, float fTime, void *pData);
	void SetAnimationCallbackHandler(int nAnimationSet, CAnimationCallbackHandler *pCallbackHandler);

	void AdvanceTime(float fElapsedTime);
};

class CLoadedModelInfo
{
public:
	CLoadedModelInfo() { }
	~CLoadedModelInfo() { }

	int 							m_nSkinnedMeshes = 0;

    CGameObject						*m_pModelRootObject = NULL;			//계층정보
	CAnimationSets					*m_pAnimationSets = NULL;			//애니메이션 정보
};

class CSkinningBoneTransforms
{
public:
	CSkinningBoneTransforms(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, CLoadedModelInfo *pModel);
	~CSkinningBoneTransforms();

public:
	int 							m_nSkinnedMeshes = 0;

	CSkinnedMesh					**m_ppSkinnedMeshes = NULL;

	ID3D12Resource					**m_ppd3dcbBoneTransforms = NULL;
	XMFLOAT4X4						**m_ppcbxmf4x4BoneTransforms = NULL;

public:
	void SetSkinnedMesh(int nIndex, CSkinnedMesh *pSkinnedMesh) { m_ppSkinnedMeshes[nIndex] = pSkinnedMesh; }

	void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	void ReleaseShaderVariables();

	void SetSkinnedMeshBoneTransformConstantBuffer();
};
/////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//가상의 네모 박스
class CCubeObject
{
public:
	CCubeObject(){}
	CCubeObject(float fWidth, float fHeight, float fDepth);
	

	virtual ~CCubeObject();


	XMFLOAT3 GetPosition() { return m_CenterPos; }
	void UpdatePosition(const XMFLOAT3 pos,float fTimeLag);
	CCubeObject& GetCubeObject() { return *this; }

	float GetDepth(){ return m_fDepth; }
	float GetWidth(){ return m_fWidth; }
	float GetHeight() { return m_fHeight; }
	
private:

	XMFLOAT3	*m_pVertex = NULL;
	int			m_nVertex ;

	XMFLOAT3	m_CenterPos;
	float		m_fWidth, m_fHeight, m_fDepth;

};



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CGameObject
{
private:
	int								m_nReferences = 0;

protected:


	int m_matID = 0;
public:
	void AddRef();
	void Release();

public:
	CGameObject();
	CGameObject(int nMaterials);
    virtual ~CGameObject();

public:

	static enum eMaterials
	{
		PINK = 0,
		BROWN,
		WHITE,
		BLACK,
		BLUE,
		PANDA,
		ICE
	};

	static enum LODLEVEL 
	{ 
		LOD_LEVEL0 = 0, 
		LOD_LEVEL1, 
		LOD_LEVEL2,
		LOD_BILLBOARD
	};



	char							m_pstrFrameName[64];

	CMesh							*m_pMesh = NULL;
	
	int								m_nMaterials = 0;
	CMaterial						**m_ppMaterials = NULL;

	XMFLOAT4X4						m_xmf4x4ToParent;
	XMFLOAT4X4						m_xmf4x4World;

	CGameObject 					*m_pParent = NULL;
	CGameObject 					*m_pChild = NULL;
	CGameObject 					*m_pSibling = NULL;

	CParticleBillboardObject        *m_pParticleObject = NULL;



	int	 GetMatID() { return m_matID; }
	
	void SetLODlevel(UINT lod_level) { m_lodLevel = lod_level; }
	void SetMesh(CMesh *pMesh);
	void SetShader(CShader *pShader);
	void SetShader(int nMaterial, CShader *pShader);
	void SetMaterial(int nMaterial, CMaterial *pMaterial);

	void SetChild(CGameObject *pChild, bool bReferenceUpdate=false);

	virtual void BuildMaterials(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList) { }

	virtual void OnPrepareAnimate() { }
	
	virtual void Animate(float fTimeElapsed, CCamera *pCamera = NULL);


	virtual void OnPrepareRender() { }
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera=NULL);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, bool bIce,int matID, CCamera *pCamera = NULL);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, UINT lodlevel, CCamera *pCamera = NULL);



	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();

	virtual void UpdateShaderVariable(ID3D12GraphicsCommandList *pd3dCommandList, XMFLOAT4X4 *pxmf4x4World);
	virtual void UpdateShaderVariable(ID3D12GraphicsCommandList *pd3dCommandList, CMaterial *pMaterial);

	virtual void ReleaseUploadBuffers();

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetLook();
	XMFLOAT3 GetUp();
	XMFLOAT3 GetRight();

	void SetPosition(float x, float y, float z);
	void SetPosition(XMFLOAT3 xmf3Position);
	void SetScale(float x, float y, float z);

	void MoveStrafe(float fDistance = 1.0f);
	void MoveUp(float fDistance = 1.0f);
	void MoveForward(float fDistance = 1.0f);

	void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);
	void Rotate(XMFLOAT3 *pxmf3Axis, float fAngle);
	void Rotate(XMFLOAT4 *pxmf4Quaternion);

	CGameObject *GetParent() { return(m_pParent); }
	void UpdateTransform(XMFLOAT4X4 *pxmf4x4Parent = NULL );
	CGameObject *FindFrame(char *pstrFrameName);

	CTexture *FindReplicatedTexture(_TCHAR *pstrTextureName);

	UINT GetMeshType() { return((m_pMesh) ? m_pMesh->GetType() : 0x00); }
	UINT GetLodLevel() { return m_lodLevel; }
public:
	CAnimationController 			*m_pAnimationController = NULL;
	CSkinningBoneTransforms 		*m_pSkinningBoneTransforms = NULL;

	CGameObject *GetRootSkinnedGameObject();

	void SetTrackAnimationSet(int nAnimationTrack, int nAnimationSet);
	void SetTrackAnimationPosition(int nAnimationTrack, float fPosition);

	void CacheSkinningBoneFrames(CGameObject *pRootFrame);
	void FindAndSetSkinnedMesh(int *pnSkinMesh, CSkinningBoneTransforms *pSkinningBoneTransforms);

	void LoadMaterialsFromFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, CGameObject *pParent, FILE *pInFile, CShader *pShader);


	static CAnimationSets *LoadAnimationFromFile(FILE *pInFile, CGameObject *pRootFrame);
	static CGameObject *LoadFrameHierarchyFromFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, CGameObject *pParent, FILE *pInFile, CShader *pShader, int *pnSkinnedMeshes);

	static CLoadedModelInfo *LoadGeometryAndAnimationFromFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, char *pstrFileName, CShader *pShader, bool bHasAnimation);
	static CGameObject *LoadGeometryFromFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, char *pstrFileName, CShader *pShader);


	static void PrintFrameInfo(CGameObject *pGameObject, CGameObject *pParent);

protected:
	UINT m_lodLevel;
};

//////////////////////////////////////////////////////////////////////////////////////////
//얼음 파편
class CIceCubeObject : public CGameObject
{
public:
	CIceCubeObject(int nMaterials);
	virtual ~CIceCubeObject();

protected:
	XMFLOAT3 m_xmf3RotationAxis;
	float m_fRotationSpeed;

	bool  m_bBlowingUp = true;

	XMFLOAT4X4 m_pxmf4x4Transforms[EXPLOSION_DEBRISES];

	float m_fMovingSpeed = 0.0f;

	float m_fElapsedTimes = 0.0f;
	float m_fDuration = 5.0f;
	float m_fExplosionSpeed = 10.0f;
	float m_fExplosionRotation = 720.0f;

	const int maxParticleCount = 60;
public:
	void SetRotationSpeed(float fRotationSpeed) { m_fRotationSpeed = fRotationSpeed; }
	void SetRotationAxis(XMFLOAT3 xmf3RotationAxis) {
		m_xmf3RotationAxis = xmf3RotationAxis;
	}

	void SetMovingSpeed(float fSpeed) { m_fMovingSpeed = fSpeed; }
	void SetExplode(bool bBlowing);

	float GetRotationSpeed() { return m_fRotationSpeed; }
	virtual void Animate(float fTimeElapsed,CCamera *pCamera =NULL);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);



public:
	static CMesh *m_pExplosionMesh;
	static XMFLOAT3 m_pxmf3SphereVectors[EXPLOSION_DEBRISES];
	
	static void PrepareExplosion(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	static XMVECTOR RandomUintVectorOnSphere();
	static int GetRandom(int min, int max);
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CHeightMapTerrain : public CGameObject
{
public:
	CHeightMapTerrain(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, LPCTSTR pFileName, int nWidth, int nLength, XMFLOAT3 xmf3Scale, XMFLOAT4 xmf4Color);
	virtual ~CHeightMapTerrain();

private:
	CHeightMapImage				*m_pHeightMapImage;

	int							m_nWidth;
	int							m_nLength;

	XMFLOAT3					m_xmf3Scale;

public:
	float GetHeight(float x, float z, bool bReverseQuad = false) { return(m_pHeightMapImage->GetHeight(x, z, bReverseQuad) * m_xmf3Scale.y); } //World
	XMFLOAT3 GetNormal(float x, float z) { return(m_pHeightMapImage->GetHeightMapNormal(int(x / m_xmf3Scale.x), int(z / m_xmf3Scale.z))); }

	int GetHeightMapWidth() { return(m_pHeightMapImage->GetHeightMapWidth()); }
	int GetHeightMapLength() { return(m_pHeightMapImage->GetHeightMapLength()); }

	XMFLOAT3 GetScale() { return(m_xmf3Scale); }
	float GetWidth() { return(m_nWidth * m_xmf3Scale.x); }
	float GetLength() { return(m_nLength * m_xmf3Scale.z); }
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CSkyBox : public CGameObject
{
public:
	CSkyBox(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature);
	virtual ~CSkyBox();

	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera = NULL);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CSuperCobraObject : public CGameObject
{
public:
	CSuperCobraObject(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature);
	virtual ~CSuperCobraObject();

private:
	CGameObject					*m_pMainRotorFrame = NULL;
	CGameObject					*m_pTailRotorFrame = NULL;

public:
	virtual void OnPrepareAnimate();
	virtual void Animate(float fTimeElapsed,CCamera *pCamera = NULL);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CGunshipObject : public CGameObject
{
public:
	CGunshipObject(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature);
	virtual ~CGunshipObject();

private:
	CGameObject					*m_pMainRotorFrame = NULL;
	CGameObject					*m_pTailRotorFrame = NULL;

public:
	virtual void OnPrepareAnimate();
	virtual void Animate(float fTimeElapsed,CCamera *pCamera = NULL);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CMi24Object : public CGameObject
{
public:
	CMi24Object(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature);
	virtual ~CMi24Object();

private:
	CGameObject					*m_pMainRotorFrame = NULL;
	CGameObject					*m_pTailRotorFrame = NULL;

public:
	virtual void OnPrepareAnimate();
	virtual void Animate(float fTimeElapsed,CCamera *pCamera=NULL);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CAngrybotObject : public CGameObject
{
public:
	CAngrybotObject(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature,int matID);
	virtual ~CAngrybotObject();

	bool m_bBloiwingUp = false;

public:
	virtual void OnPrepareAnimate();
	virtual void Animate(float fTimeElapsed,CCamera *pCamera = NULL);



};

class CDeadTreeObject : public CGameObject
{
public:
	CDeadTreeObject(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature);
	virtual ~CDeadTreeObject();


};



class CPineTreeObject : public CGameObject
{
public:
	CPineTreeObject(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature);
	virtual ~CPineTreeObject();


};

class CDeerObject : public CGameObject
{
public:
	CDeerObject(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature);
	virtual ~CDeerObject();


};


class CFoliageObject : public CGameObject
{
public:
	CFoliageObject(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature);
	virtual ~CFoliageObject();
	

	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, UINT lodlevel,CCamera *pCamera = NULL);
	
public:
	static CGameObject *LoadFrameHierarchyFromFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, CGameObject *pParent, FILE *pInFile, CShader *pShader, int *pnSkinnedMeshes);
	static CLoadedModelInfo *LoadGeometryAndAnimationFromFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, char *pstrFileName, CShader *pShader, bool bHasAnimation);

	

};


/////////////////////////////////////////////////////////////////////////////////////////

class CBillboardObject : public CGameObject
{
public:
	CBillboardObject();
	virtual ~CBillboardObject();
public:
	virtual void Animate(float fTimeElapsed, CCamera *pCamera =NULL);
	//virtual void UpdateTransform( CCamera *pCamera = NULL, XMFLOAT4X4 *pxmf4x4Parent = NULL);
	void SetOffsetX(float offset) { m_offsetX = offset; }
	void SetOffsetZ(float offset) { m_offsetZ = offset; }


	void SetPositionXZ(float x, float z);
	void SetLookAt(XMFLOAT3& xmfTarget);

	float GetOffsetX() { return m_offsetX; }
	float GetOffsetZ() { return m_offsetZ; }
private:
	float						m_offsetX = 0.0f;
	float						m_offsetZ = 0.0f;
};



////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CSnowBillboardObject : public CBillboardObject
{
public:
	CSnowBillboardObject();
	virtual ~CSnowBillboardObject();

public:
	virtual void Animate(float fTimeElapsed, CCamera *pCamera = NULL);
	
			//플레이어 중심으로부터 얼마나 떨어져 있는지 변수
};

///////////////////////////////////////////////////////////////////////////////////
class CGrassFoliageBillboardObject : public CBillboardObject
{
public:
	CGrassFoliageBillboardObject(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphcisRoogSignature);
	virtual ~CGrassFoliageBillboardObject();

	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera = NULL);
};


//////////////////////////////////////////////////////////////////////////////////

class CParticleBillboardObject : public CBillboardObject
{
public:
	CParticleBillboardObject(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphcisRoogSignature);
	virtual ~CParticleBillboardObject();

public:
	//virtual void UpdateTransform(CCamera *pCamera = NULL, XMFLOAT4X4 *pxmf4x4Parent = NULL);

	virtual void Animate(float fTimeElapsed, CCamera *pCamera = NULL);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera = NULL);

	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();

	virtual void UpdateShaderVariable(ID3D12GraphicsCommandList *pd3dCommandList, XMFLOAT4X4 *pxmf4x4World);


protected:
	UINT						m_fParticleTime = 0.0f;

	D3D12_CPU_DESCRIPTOR_HANDLE		m_d3dCbvCPUDescriptorStartHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE		m_d3dCbvGPUDescriptorStartHandle;

	CB_PARTICLE_INFO				*m_pcbMappedParticlObjects = NULL;
	ID3D12Resource					*m_pd3dcbParticleObjects = NULL;
};
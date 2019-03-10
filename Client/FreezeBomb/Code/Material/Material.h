#pragma once

#define MATERIAL_ALBEDO_MAP				0x01
#define MATERIAL_SPECULAR_MAP				0x02
#define MATERIAL_NORMAL_MAP				0x04
#define MATERIAL_METALLIC_MAP				0x08
#define MATERIAL_EMISSION_MAP				0x10
#define MATERIAL_DETAIL_ALBEDO_MAP	0x20
#define MATERIAL_DETAIL_NORMAL_MAP	0x40

struct CB_MATERIAL_INFO
{
	// g_Material
	XMFLOAT4	m_xmf4AmbientColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4	m_xmf4AlbedoColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	XMFLOAT4	m_xmf4SpecularColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4	m_xmf4EmissiveColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	// gnTexturesMask
	UINT				m_nType = 0x00;
};

struct CB_GAMEOBJECT_INFO;
class CTexture;
class CShader;
class CGameObject;

class CMaterial
{
public:
	CMaterial();
	CMaterial(int nTextures);
	virtual ~CMaterial();

private:
	int								m_nReferences = 0;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

public:
	CShader*					m_pShader = NULL;

	XMFLOAT4				m_xmf4AmbientColor	= XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4				m_xmf4AlbedoColor		= XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	XMFLOAT4				m_xmf4SpecularColor	= XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4				m_xmf4EmissiveColor	= XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

	UINT							m_nType = 0x00;

	void SetShader(CShader *pShader);
	void SetMaterialType(UINT nType) { m_nType |= nType; }
	void SetTexture(CTexture *pTexture, UINT nTexture = 0);

	virtual void UpdateShaderVariable(ID3D12GraphicsCommandList *pd3dCommandList);

	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();

	virtual void ReleaseUploadBuffers();

public:

	float							m_fGlossiness = 0.0f;
	float							m_fSmoothness = 0.0f;
	float							m_fSpecularHighlight = 0.0f;
	float							m_fMetallic = 0.0f;
	float							m_fGlossyReflection = 0.0f;

public:
	int 							m_nTextures = 0;
	_TCHAR(*m_ppstrTextureNames)[64] = NULL;
	CTexture**				m_ppTextures = NULL;		//0:Albedo, 1:Specular, 2:Metallic, 3:Normal, 4:Emission, 5:DetailAlbedo, 6:DetailNormal

	void LoadTextureFromFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, UINT nType, UINT nRootParameter, _TCHAR *pwstrTextureName, CTexture **ppTexture, CGameObject *pParent, FILE *pInFile, CShader *pShader);

public:
	static CShader* m_pStandardShader;
	static CShader* m_pSkinnedAnimationShader;

	static void PrepareShaders(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature);

	void SetStandardShader()	{ CMaterial::SetShader(m_pStandardShader); }
	void SetSkinnedAnimationShader() { CMaterial::SetShader(m_pSkinnedAnimationShader); }

private:
	// »ó¼ö¹öÆÛºä
	ID3D12Resource*			m_pd3dcbMaterial { nullptr };
	CB_MATERIAL_INFO*	m_pcbMappedMaterial { nullptr };
};
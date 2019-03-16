#include "../Stdafx/Stdafx.h"
#include "GameObject.h"
#include "../Texture/Texture.h"
#include "../Material/Material.h"
#include "../Shader/Shader.h"
#include "Billboard/LampParticle/LampParticle.h"
#include "../SoundSystem/SoundSystem.h"

//int CGameObject::m_AnimationType = CGameObject::ANIMATIONTYPE::IDLE;

class CSoundSystem;
CAnimationSet::CAnimationSet()
{
}

CAnimationSet::~CAnimationSet()
{
	if (m_pfKeyFrameTransformTimes) delete[] m_pfKeyFrameTransformTimes;
	for (int j = 0; j < m_nKeyFrameTransforms; j++) if (m_ppxmf4x4KeyFrameTransforms[j]) delete[] m_ppxmf4x4KeyFrameTransforms[j];
	if (m_ppxmf4x4KeyFrameTransforms) delete[] m_ppxmf4x4KeyFrameTransforms;

	if (m_pCallbackKeys) delete[] m_pCallbackKeys;
	if (m_pAnimationCallbackHandler) delete m_pAnimationCallbackHandler;
}

void *CAnimationSet::GetCallbackData()
{
	// 애니메이션이 동작하는 시간과 읽어가는 위치를 비교하여, 소리를 재생하게 만듬.
	// 근사값으로 비교하여 CallbackKey를 만든다. => ANIMATION_CALLBACK_EPSILON : 근사값
	// 근사값이 작을수록 소리의 재생이 정확하지만, 프레임레이트의 영향에따라 달라짐.
	for (int i = 0; i < m_nCallbackKeys; i++)
	{
		if (::IsEqual(m_pCallbackKeys[i].m_fTime, m_fPosition, ANIMATION_CALLBACK_EPSILON)) return(m_pCallbackKeys[i].m_pCallbackData);
	}
	return(NULL);
}


void CAnimationSet::SetPosition(CAnimationController& AnimationController,float& fTrackPosition)
{
	m_fPosition = fTrackPosition;

	switch (m_nType)
	{
	case ANIMATION_TYPE_LOOP:
	{
#ifdef _WITH_ANIMATION_INTERPOLATION			
		//fmod 
		//0~1사이의 값이 됨
		m_fPosition = fmod(fTrackPosition, m_pfKeyFrameTransformTimes[m_nKeyFrameTransforms - 1]); // m_fPosition = fTrackPosition - int(fTrackPosition / m_pfKeyFrameTransformTimes[m_nKeyFrameTransforms-1]) * m_pfKeyFrameTransformTimes[m_nKeyFrameTransforms-1];
		//m_fPosition = fmod(fTrackPosition, m_fLength); //if (m_fPosition < 0) m_fPosition += m_fLength;
		//m_fPosition = fTrackPosition - int(fTrackPosition / m_fLength) * m_fLength;
#else
		m_nCurrentKey++;
		if (m_nCurrentKey >= m_nKeyFrameTransforms) m_nCurrentKey = 0;
#endif
		break;
	}
	case ANIMATION_TYPE_ONCE:
		if (fTrackPosition >= m_fLength - 0.1f)
		{
			if (AnimationController.m_state == CAnimationController::DIE)
			{
				AnimationController.m_state = CAnimationController::DIE;
				fTrackPosition = m_fLength - 0.1f;
			}
			else
			{
				AnimationController.m_state = CAnimationController::IDLE;
				fTrackPosition = 0.0f;
			}
			
		}
		break;

	case ANIMATION_TYPE_PINGPONG:
		break;
}

	if (m_pAnimationCallbackHandler)
	{
		void *pCallbackData = GetCallbackData();
		if (pCallbackData)
		{
			m_pAnimationCallbackHandler->HandleCallback(pCallbackData);
		}
	}
}

XMFLOAT4X4 CAnimationSet::GetSRT(int nFrame)
{
	XMFLOAT4X4 xmf4x4Transform = Matrix4x4::Identity();
#ifdef _WITH_ANIMATION_INTERPOLATION
#ifdef _WITH_ANIMATION_SRT
	XMVECTOR S, R, T;
	for (int i = 0; i < (m_nKeyFrameTranslations - 1); i++)
	{
		if ((m_pfKeyFrameTranslationTimes[i] <= m_fPosition) && (m_fPosition <= m_pfKeyFrameTranslationTimes[i+1]))
		{
			float t = (m_fPosition - m_pfKeyFrameTranslationTimes[i]) / (m_pfKeyFrameTranslationTimes[i+1] - m_pfKeyFrameTranslationTimes[i]);
			T = XMVectorLerp(XMLoadFloat3(&m_ppxmf3KeyFrameTranslations[i][nFrame]), XMLoadFloat3(&m_ppxmf3KeyFrameTranslations[i+1][nFrame]), t);
			break;
		}
	}
	for (UINT i = 0; i < (m_nKeyFrameScales - 1); i++)
	{
		if ((m_pfKeyFrameScaleTimes[i] <= m_fPosition) && (m_fPosition <= m_pfKeyFrameScaleTimes[i+1]))
		{
			float t = (m_fPosition - m_pfKeyFrameScaleTimes[i]) / (m_pfKeyFrameScaleTimes[i+1] - m_pfKeyFrameScaleTimes[i]);
			S = XMVectorLerp(XMLoadFloat3(&m_ppxmf3KeyFrameScales[i][nFrame]), XMLoadFloat3(&m_ppxmf3KeyFrameScales[i+1][nFrame]), t);
			break;
		}
	}
	for (UINT i = 0; i < (m_nKeyFrameRotations - 1); i++)
	{
		if ((m_pfKeyFrameRotationTimes[i] <= m_fPosition) && (m_fPosition <= m_pfKeyFrameRotationTimes[i+1]))
		{
			float t = (m_fPosition - m_pfKeyFrameRotationTimes[i]) / (m_pfKeyFrameRotationTimes[i+1] - m_pfKeyFrameRotationTimes[i]);
			R = XMQuaternionSlerp(XMLoadFloat4(&m_ppxmf4KeyFrameRotations[i][nFrame]), XMLoadFloat4(&m_ppxmf4KeyFrameRotations[i+1][nFrame]), t);
			break;
		}
	}

	XMStoreFloat4x4(&xmf4x4Transform, XMMatrixAffineTransformation(S, NULL, R, T));
#else   
	for (int i = 0; i < (m_nKeyFrameTransforms - 1); i++) 
	{
		if ((m_pfKeyFrameTransformTimes[i] <= m_fPosition) && (m_fPosition <= m_pfKeyFrameTransformTimes[i+1]))
		{
			float t = (m_fPosition - m_pfKeyFrameTransformTimes[i]) / (m_pfKeyFrameTransformTimes[i+1] - m_pfKeyFrameTransformTimes[i]);
			xmf4x4Transform = Matrix4x4::Interpolate(m_ppxmf4x4KeyFrameTransforms[i][nFrame], m_ppxmf4x4KeyFrameTransforms[i + 1][nFrame], t);
			break;
		}
	}
#endif
#else
	xmf4x4Transform = m_ppxmf4x4KeyFrameTransforms[m_nCurrentKey][nFrame];
#endif
	return(xmf4x4Transform);
}

void CAnimationSet::SetCallbackKeys(int nCallbackKeys)
{
	m_nCallbackKeys = nCallbackKeys;
	m_pCallbackKeys = new CALLBACKKEY[nCallbackKeys];
}

void CAnimationSet::SetCallbackKey(int nKeyIndex, float fKeyTime, void *pData)
{
	m_pCallbackKeys[nKeyIndex].m_fTime = fKeyTime;
	m_pCallbackKeys[nKeyIndex].m_pCallbackData = pData;
}

void CAnimationSet::SetAnimationCallbackHandler(CAnimationCallbackHandler *pCallbackHandler,void* pContext)
{
	m_pAnimationCallbackHandler = pCallbackHandler;
	if (pContext)
	{
		m_pAnimationCallbackHandler->SetAdditianalData(pContext);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CAnimationSets::CAnimationSets()
{
}

CAnimationSets::~CAnimationSets()
{
	if (m_pAnimationSets) delete[] m_pAnimationSets;
	if (m_ppAnimationFrameCaches) delete[] m_ppAnimationFrameCaches;
}

void CAnimationSets::SetCallbackKeys(int nAnimationSet, int nCallbackKeys)
{
	m_pAnimationSets[nAnimationSet].m_nCallbackKeys = nCallbackKeys;
	m_pAnimationSets[nAnimationSet].m_pCallbackKeys = new CALLBACKKEY[nCallbackKeys];
}

void CAnimationSets::SetCallbackKey(int nAnimationSet, int nKeyIndex, float fKeyTime, void *pData)
{
	m_pAnimationSets[nAnimationSet].m_pCallbackKeys[nKeyIndex].m_fTime = fKeyTime;
	m_pAnimationSets[nAnimationSet].m_pCallbackKeys[nKeyIndex].m_pCallbackData = pData;
}

void CAnimationSets::SetAnimationCallbackHandler(int nAnimationSet, CAnimationCallbackHandler *pCallbackHandler,void* pSoundContext)
{
	m_pAnimationSets[nAnimationSet].SetAnimationCallbackHandler(pCallbackHandler,pSoundContext);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//

CAnimationController::CAnimationController(int nAnimationTracks, CAnimationSets *pAnimationSets)
{
	m_nAnimationTracks = nAnimationTracks;
    m_pAnimationTracks = new CAnimationTrack[nAnimationTracks];

	
	SetAnimationSets(pAnimationSets);
}

CAnimationController::~CAnimationController()
{
	if (m_pAnimationSets) m_pAnimationSets->Release();

	if (m_pAnimationTracks) delete[] m_pAnimationTracks;
}

void CAnimationController::SetCallbackKeys(int nAnimationSet, int nCallbackKeys)
{
	if (m_pAnimationSets) m_pAnimationSets->SetCallbackKeys(nAnimationSet, nCallbackKeys);
}

void CAnimationController::SetCallbackKey(int nAnimationSet, int nKeyIndex, float fKeyTime, void *pData)
{
	if (m_pAnimationSets) m_pAnimationSets->SetCallbackKey(nAnimationSet, nKeyIndex, fKeyTime, pData);
}

void CAnimationController::SetAnimationCallbackHandler(int nAnimationSet, CAnimationCallbackHandler *pCallbackHandler,void* pSoundContext)
{
	if (m_pAnimationSets) m_pAnimationSets->SetAnimationCallbackHandler(nAnimationSet, pCallbackHandler,pSoundContext);
}

void CAnimationController::SetTrackAnimationSet(int nAnimationTrack, int nAnimationSet)
{
	if (m_pAnimationSets && (nAnimationSet < m_pAnimationSets->m_nAnimationSets))
	{
		if (m_pAnimationTracks) m_pAnimationTracks[nAnimationTrack].SetAnimationSet(&m_pAnimationSets->m_pAnimationSets[nAnimationSet]);
	}
}

void CAnimationController::SetTrackEnable(int nAnimationTrack, bool bEnable)
{
	if (m_pAnimationTracks) m_pAnimationTracks[nAnimationTrack].SetEnable(bEnable);
}

void CAnimationController::SetTrackPosition(int nAnimationTrack, float fPosition)
{
	if (m_pAnimationTracks) m_pAnimationTracks[nAnimationTrack].SetPosition(fPosition);
}

float CAnimationController::GetTrackPosition(int nAnimationTrack)
{
	if (m_pAnimationTracks)
		return m_pAnimationTracks[nAnimationTrack].GetPosition();
	else
		return 0.f;
}

void CAnimationController::SetTrackSpeed(int nAnimationTrack, float fSpeed)
{
	if (m_pAnimationTracks) m_pAnimationTracks[nAnimationTrack].SetSpeed(fSpeed);
}

void CAnimationController::SetTrackWeight(int nAnimationTrack, float fWeight)
{
	if (m_pAnimationTracks) m_pAnimationTracks[nAnimationTrack].SetWeight(fWeight);
}

void CAnimationController::SetAnimationSets(CAnimationSets *pAnimationSets)
{
	m_pAnimationSets = pAnimationSets;
	pAnimationSets->AddRef();
}

void CAnimationController::AdvanceTime(float fTimeElapsed) 
{
	m_fTime += fTimeElapsed; 
	if (m_pAnimationSets && m_pAnimationTracks)
	{
		for (int i = 0; i < m_pAnimationSets->m_nAnimationFrames; i++)
		{
			m_pAnimationSets->m_ppAnimationFrameCaches[i]->m_xmf4x4ToParent = Matrix4x4::Zero();
		}
		for (int j = 0; j < m_nAnimationTracks; j++)
		{
			m_pAnimationTracks[j].m_fPosition += (fTimeElapsed * m_pAnimationTracks[j].m_fSpeed);		//각 애니메이션 트랙의 속도와 게임 경과시간을 곱해서 해당 애니메이션이 어느 시간대를 가리키는지 정해야한다.

			CAnimationSet *pAnimationSet = m_pAnimationTracks[j].m_pAnimationSet;
			pAnimationSet->m_fPosition += (fTimeElapsed * m_pAnimationTracks[j].m_fSpeed);
			m_pAnimationTracks[j].m_pAnimationSet->SetPosition(*this,m_pAnimationTracks[j].m_fPosition);
			//m_pAnimationTracks[j].m_pAnimationSet->m_fLength;
			if (m_pAnimationTracks[j].m_bEnable)
			{
				for (int i = 0; i < m_pAnimationSets->m_nAnimationFrames; i++)
				{
					//m_pAnimationSets->m_ppAnimationFrameCaches[i]->m_xmf4x4ToParent = Matrix4x4::Add(m_pAnimationSets->m_ppAnimationFrameCaches[i]->m_xmf4x4ToParent, Matrix4x4::Scale(m_pAnimationTracks[j].m_pAnimationSet->GetSRT(i), m_pAnimationTracks[j].m_fWeight));
					//프레임 마다 그 시간대의 변환행렬이 있는지 확인후 있다면 변환
					m_pAnimationSets->m_ppAnimationFrameCaches[i]->m_xmf4x4ToParent = Matrix4x4::Scale(m_pAnimationTracks[j].m_pAnimationSet->GetSRT(i), m_pAnimationTracks[j].m_fWeight);

				}
			}
		}
	}
} 

//#define _WITH_DEBUG_CALLBACK_DATA
//#define _WITH_SOUND_RESOURCE
void CSoundCallbackHandler::HandleCallback(void *pCallbackData)
{
	//_TCHAR *pWavName = (_TCHAR *)pCallbackData;
//#ifdef _WITH_DEBUG_CALLBACK_DATA
//	TCHAR pstrDebug[256] = { 0 };
//	_stprintf_s(pstrDebug, 256, _T("%s\n"), pWavName);
//	OutputDebugString(pstrDebug);
//#endif
//#ifdef _WITH_SOUND_RESOURCE
//	PlaySound(MAKEINTRESOURCE(pCallbackData), ::ghAppInstance, SND_RESOURCE | SND_ASYNC);
//#else
//	PlaySound(pWavName, NULL, SND_FILENAME | SND_ASYNC);
//#endif

	if (m_pContextData) {
		CSoundSystem* pSound = (CSoundSystem*)m_pContextData;

		//사운드 배열은 0부터지만 넘어오는 데이터는 1부터 시작하기 때문에 인자에서 1빼서 넘겨준다.
		pSound->PlayIndex((int)pCallbackData-1);
	}
}


CSkinningBoneTransforms::CSkinningBoneTransforms(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, CLoadedModelInfo *pModel)
{
	m_nSkinnedMeshes = pModel->m_nSkinnedMeshes;

	// SkinnedMesh가 여러개 일 경우
	m_ppSkinnedMeshes = new CSkinnedMesh*[m_nSkinnedMeshes];

	m_ppd3dcbBoneTransforms = new ID3D12Resource*[m_nSkinnedMeshes];
	m_ppcbxmf4x4BoneTransforms = new XMFLOAT4X4*[m_nSkinnedMeshes];

	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	int nSkinnedMesh = 0;
	pModel->m_pModelRootObject->FindAndSetSkinnedMesh(&nSkinnedMesh, this);
}

CSkinningBoneTransforms::~CSkinningBoneTransforms()
{
	ReleaseShaderVariables();

	if (m_ppSkinnedMeshes) delete[] m_ppSkinnedMeshes;
	if (m_ppd3dcbBoneTransforms) delete[] m_ppd3dcbBoneTransforms;
	if (m_ppcbxmf4x4BoneTransforms) delete[] m_ppcbxmf4x4BoneTransforms;
}

void CSkinningBoneTransforms::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = (((sizeof(XMFLOAT4X4) * SKINNED_ANIMATION_BONES) + 255) & ~255); //256의 배수
	for (int i = 0; i < m_nSkinnedMeshes; i++)
	{
		m_ppd3dcbBoneTransforms[i] = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
		m_ppd3dcbBoneTransforms[i]->Map(0, NULL, (void **)&m_ppcbxmf4x4BoneTransforms[i]);
	}
}

void CSkinningBoneTransforms::SetSkinnedMeshBoneTransformConstantBuffer()
{
	for (int i = 0; i < m_nSkinnedMeshes; i++)
	{
		m_ppSkinnedMeshes[i]->m_pd3dcbBoneTransforms = m_ppd3dcbBoneTransforms[i];
		m_ppSkinnedMeshes[i]->m_pcbxmf4x4BoneTransforms = m_ppcbxmf4x4BoneTransforms[i];
	}
}

void CSkinningBoneTransforms::ReleaseShaderVariables()
{
	for (int i = 0; i < m_nSkinnedMeshes; i++)
	{
		m_ppd3dcbBoneTransforms[i]->Unmap(0, NULL);
		m_ppd3dcbBoneTransforms[i]->Release();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CGameObject::CGameObject()
{
	m_xmf4x4ToParent = Matrix4x4::Identity();
	m_xmf4x4World = Matrix4x4::Identity();
}

CGameObject::CGameObject(int nMaterials) : CGameObject()
{
	m_nMaterials = nMaterials;
	if (m_nMaterials > 0)
	{
		m_ppMaterials = new CMaterial*[m_nMaterials];
		for (int i = 0; i < m_nMaterials; i++)
		{
			m_ppMaterials[i] = NULL;
		}
	}
}

CGameObject::~CGameObject()
{
	if (m_pMesh) 
		m_pMesh->Release();

	if (m_nMaterials > 0)
	{
		for (int i = 0; i < m_nMaterials; i++)
		{
			if (m_ppMaterials[i]) 
				m_ppMaterials[i]->Release();
		}
	}
	if (m_ppMaterials) 
		delete[] m_ppMaterials;

	if (m_pAnimationController) 
		delete m_pAnimationController;
	if (m_pSkinningBoneTransforms) 
		delete m_pSkinningBoneTransforms;
}

void CGameObject::AddRef() 
{ 
	m_nReferences++; 

	if (m_pSibling) 
		m_pSibling->AddRef();
	if (m_pChild) 
		m_pChild->AddRef();
}

void CGameObject::Release() 
{ 
	if (m_pChild) 
		m_pChild->Release();
	if (m_pSibling) 
		m_pSibling->Release();

	if (--m_nReferences <= 0) 
		delete this; 
}

void CGameObject::SetChild(CGameObject* pChild, bool bReferenceUpdate)
{
	if (pChild)
	{
		pChild->m_pParent = this;
		if (bReferenceUpdate) 
			pChild->AddRef();
	}
	if (m_pChild)
	{
		if (pChild) 
			pChild->m_pSibling = m_pChild->m_pSibling;
		m_pChild->m_pSibling = pChild;
	}
	else
	{
		m_pChild = pChild;
	}
}

void CGameObject::SetMesh(CMesh* pMesh)
{
	if (m_pMesh) 
		m_pMesh->Release();

	m_pMesh = pMesh;

	if (m_pMesh) 
		m_pMesh->AddRef();
}

void CGameObject::SetShader(CShader *pShader)
{
	m_nMaterials = 1;
	m_ppMaterials = new CMaterial*[m_nMaterials];
	m_ppMaterials[0] = new CMaterial(0);
	m_ppMaterials[0]->SetShader(pShader);
}

void CGameObject::SetShader(int nMaterial, CShader *pShader)
{
	if (m_ppMaterials[nMaterial]) 
		m_ppMaterials[nMaterial]->SetShader(pShader);
}

void CGameObject::SetMaterial(int nMaterial, CMaterial *pMaterial)
{
	if (m_ppMaterials[nMaterial]) m_ppMaterials[nMaterial]->Release();
	m_ppMaterials[nMaterial] = pMaterial;
	if (m_ppMaterials[nMaterial]) m_ppMaterials[nMaterial]->AddRef();

}

CGameObject *CGameObject::FindFrame(char *pstrFrameName)
{
	CGameObject *pFrameObject = NULL;
	if (!strncmp(m_pstrFrameName, pstrFrameName, strlen(pstrFrameName))) return(this);

	if (m_pSibling) 
		if (pFrameObject = m_pSibling->FindFrame(pstrFrameName)) 
			return(pFrameObject);
	if (m_pChild) 
		if (pFrameObject = m_pChild->FindFrame(pstrFrameName))
			return(pFrameObject);

	return(NULL);
}

CGameObject *CGameObject::GetRootSkinnedGameObject()
{
	if (m_pAnimationController) 
		return this;

	CGameObject *pRootSkinnedGameObject = NULL;
	if (m_pSibling) 
		if (pRootSkinnedGameObject = m_pSibling->GetRootSkinnedGameObject()) 
			return(pRootSkinnedGameObject);
	if (m_pChild) 
		if (pRootSkinnedGameObject = m_pChild->GetRootSkinnedGameObject()) 
			return(pRootSkinnedGameObject);

	return(NULL);
}

void CGameObject::UpdateTransform(XMFLOAT4X4 *pxmf4x4Parent)
{
	m_xmf4x4World = (pxmf4x4Parent) ? Matrix4x4::Multiply(m_xmf4x4ToParent, *pxmf4x4Parent) : m_xmf4x4ToParent;

	if (m_pSibling) 
		m_pSibling->UpdateTransform(pxmf4x4Parent);
	if (m_pChild) 
		m_pChild->UpdateTransform(&m_xmf4x4World);
}

void CGameObject::SetTrackAnimationSet(int nAnimationTrack, int nAnimationSet)
{
	if (m_pAnimationController) 
		m_pAnimationController->SetTrackAnimationSet(nAnimationTrack, nAnimationSet);
}

void CGameObject::SetTrackAnimationPosition(int nAnimationTrack, float fPosition)
{
	if (m_pAnimationController) 
		m_pAnimationController->SetTrackPosition(nAnimationTrack, fPosition);
}

float CGameObject::GetTrackAnimationPosition(int nAnimationTrack)
{
	if (m_pAnimationController)
		return m_pAnimationController->GetTrackPosition(nAnimationTrack);
	else
		return 0.f;
}

void CGameObject::Animate(float fTimeElapsed)
{
	// 루트 오브젝트를 제외한 나머지는 모두 nullptr이다. 왜냐하면, 루트 오브젝트를 제어하기 위함이므로
	if (m_pAnimationController) 
		m_pAnimationController->AdvanceTime(fTimeElapsed);

	m_xmOOBBTransformed.Transform(m_xmOOBB, XMLoadFloat4x4(&m_xmf4x4World));
	XMStoreFloat4(&m_xmOOBBTransformed.Orientation, XMQuaternionNormalize(XMLoadFloat4(&m_xmOOBBTransformed.Orientation)));

	if (m_pSibling) 
		m_pSibling->Animate(fTimeElapsed);
	if (m_pChild) 
		m_pChild->Animate(fTimeElapsed);
}

void CGameObject::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera, int nPipelineState)
{
	OnPrepareRender();

	if (m_pSkinningBoneTransforms)
		m_pSkinningBoneTransforms->SetSkinnedMeshBoneTransformConstantBuffer();

	if (m_pMesh)
	{
		if (!m_pSkinningBoneTransforms)
			UpdateShaderVariable(pd3dCommandList, &m_xmf4x4World);

		if (m_nMaterials > 0)
		{
			for (int i = 0; i < m_nMaterials; i++)
			{
				if (m_ppMaterials[i])
				{
					if (m_ppMaterials[i]->m_pShader) 
						m_ppMaterials[i]->m_pShader->Render(pd3dCommandList, pCamera, nPipelineState);

					m_ppMaterials[i]->UpdateShaderVariables(pd3dCommandList);
				}
				m_pMesh->Render(pd3dCommandList, i);
			}
		}
	}

	if (m_pSibling) 
		m_pSibling->Render(pd3dCommandList, pCamera, nPipelineState);
	if (m_pChild) 
		m_pChild->Render(pd3dCommandList, pCamera, nPipelineState);
}


//플레이어 Render로 부터 호출되서 플레이어의 프레임들을 호출하며 Render할때 호출되는 함수
// 플레이어가 들고있는 폭탄이나 , 망치가 이 함수 Render로 호출된다.

void CGameObject::Render(ID3D12GraphicsCommandList *pd3dCommandList,bool bHammer,bool bBomb, bool bIce, int matID, CCamera* pCamera, int nPipelineState)
{
	OnPrepareRender();
	if (m_pSkinningBoneTransforms)
		m_pSkinningBoneTransforms->SetSkinnedMeshBoneTransformConstantBuffer();

	if (m_pMesh)
	{
		if (!m_pSkinningBoneTransforms)
			UpdateShaderVariable(pd3dCommandList, &m_xmf4x4World);
				
		if (m_nMaterials == 1)			//폭탄과 같은 재질이 하나 있는 오브젝트 렌더
		{
			if (m_ppMaterials[0])
			{
				if (m_ppMaterials[0]->m_pShader)
				{
					m_ppMaterials[0]->m_pShader->Render(pd3dCommandList, pCamera, nPipelineState);
				}
				m_ppMaterials[0]->UpdateShaderVariables(pd3dCommandList);
			}
		}
		else if (m_nMaterials == 2)		//망치의 재질 개수 2개 , 일반 망치, 황금 망치
		{
			if (m_ppMaterials[1])
			{
				if (m_ppMaterials[1]->m_pShader) 
					m_ppMaterials[0]->m_pShader->Render(pd3dCommandList, pCamera, nPipelineState);
				m_ppMaterials[1]->UpdateShaderVariables(pd3dCommandList);
			}
		}
		else if (m_nMaterials == 7)		
		{
			if (bIce)							//얼음 상태일때
			{
				if (m_ppMaterials[MATERIALTYPE::ICEMAT])
				{
					if (m_ppMaterials[MATERIALTYPE::ICEMAT]->m_pShader)
					{
						m_ppMaterials[MATERIALTYPE::ICEMAT]->m_pShader->Render(pd3dCommandList, pCamera, nPipelineState);
					}
					m_ppMaterials[MATERIALTYPE::ICEMAT ]->UpdateShaderVariables(pd3dCommandList);
				}
			}
			else
			{
				if (m_ppMaterials[matID])		//얼음상태가 아닐때 
				{
					if (m_ppMaterials[matID]->m_pShader)
					{
						m_ppMaterials[matID]->m_pShader->Render(pd3dCommandList, pCamera, nPipelineState);
					}
						m_ppMaterials[matID]->UpdateShaderVariables(pd3dCommandList);
				}
			}
		}
		m_pMesh->Render(pd3dCommandList, 0);
	}

	if (!strcmp(this->m_pstrFrameName, "Lamp"))		//불꽅 파티클은 일반 GameObject Render하는 방식을 해야한다. 
	{
		if (m_pSibling) 
			m_pSibling->Render(pd3dCommandList, pCamera, nPipelineState);
		if (m_pChild) 
			m_pChild->Render(pd3dCommandList, pCamera, nPipelineState);
	}
	else
	{
		if (m_pSibling)
		{
			if (!strncmp(m_pSibling->m_pstrFrameName, "Hammer", strlen(m_pSibling->m_pstrFrameName)))
			{
				if (bHammer) 
				{
					m_pSibling->Render(pd3dCommandList, bHammer, bBomb, bIce, matID, pCamera, nPipelineState);
				}
			}
			else 
			{
				m_pSibling->Render(pd3dCommandList, bHammer, bBomb, bIce, matID, pCamera, nPipelineState);
			}
			
		}
		if (m_pChild)
		{
			if (!strncmp(m_pChild->m_pstrFrameName, "black-handbomb", strlen(m_pChild->m_pstrFrameName)))
			{
				if (bBomb)
				{
					 m_pChild->Render(pd3dCommandList,bHammer, bBomb, bIce, matID, pCamera, nPipelineState);
				}
			}
			else 
			{
				m_pChild->Render(pd3dCommandList, bHammer,bBomb, bIce, matID, pCamera, nPipelineState);
			}
		}
	}	
}
//LOD 오브젝트 렌더 

void CGameObject::Render(ID3D12GraphicsCommandList *pd3dCommandList, UINT lodLevel, CCamera *pCamera, int nPipelineState)
{
	OnPrepareRender();

	if (m_pMesh)
	{
		if (!m_pSkinningBoneTransforms) UpdateShaderVariable(pd3dCommandList, &m_xmf4x4World);

		if (m_nMaterials > 0)
		{
			for (int i = 0; i < m_nMaterials; i++)
			{
				if (m_ppMaterials[i])
				{
					if (m_ppMaterials[i]->m_pShader) 
						m_ppMaterials[i]->m_pShader->Render(pd3dCommandList, pCamera, nPipelineState);
					m_ppMaterials[i]->UpdateShaderVariables(pd3dCommandList);
				}

				m_pMesh->Render(pd3dCommandList, i, lodLevel);
			}
		}
	}

	if (m_pSibling) 
		m_pSibling->Render(pd3dCommandList, lodLevel, pCamera, nPipelineState);
	if (m_pChild) 
		m_pChild->Render(pd3dCommandList, lodLevel, pCamera, nPipelineState);
}


void CGameObject::UpdateShaderVariable(ID3D12GraphicsCommandList *pd3dCommandList, XMFLOAT4X4* pxmf4x4World)
{
	XMFLOAT4X4 xmf4x4World;
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(pxmf4x4World)));
	pd3dCommandList->SetGraphicsRoot32BitConstants(1, 16, &xmf4x4World, 0);

	// 그림자 행렬 루트상수로 넘김
	XMFLOAT4 xmf4Light(0.57735f, -0.57735f, 0.57735f, 0);
	// Plane의 w 벡터가 그림자의 y에 영향을 준다.
	XMFLOAT4 xmf4Plane(0.f, 1.f, 0.f, 0.f);
	// 그림자 행렬 생성
	XMMATRIX xmmtxPlane = XMMatrixShadow(XMLoadFloat4(&xmf4Plane), -XMLoadFloat4(&xmf4Light));
	XMFLOAT4X4 ShadowWorld;
	XMStoreFloat4x4(&ShadowWorld, XMMatrixTranspose(xmmtxPlane));
	pd3dCommandList->SetGraphicsRoot32BitConstants(21, 16, &ShadowWorld, 0);
}

void CGameObject::UpdateShaderVariable(ID3D12GraphicsCommandList *pd3dCommandList, CMaterial* pMaterial)
{
}

void CGameObject::ReleaseShaderVariables()
{
}

void CGameObject::ReleaseUploadBuffers()
{
	if (m_pMesh) 
		m_pMesh->ReleaseUploadBuffers();

	for (int i = 0; i < m_nMaterials; ++i)
	{
		if (m_ppMaterials[i]) 
			m_ppMaterials[i]->ReleaseUploadBuffers();
	}

	if (m_pSibling) 
		m_pSibling->ReleaseUploadBuffers();
	if (m_pChild) 
		m_pChild->ReleaseUploadBuffers();
}

void CGameObject::SetPosition(float x, float y, float z)
{
	m_xmf4x4ToParent._41 = x;
	m_xmf4x4ToParent._42 = y;
	m_xmf4x4ToParent._43 = z;

	UpdateTransform(NULL);
}

void CGameObject::SetPosition(XMFLOAT3 xmf3Position)
{
	SetPosition(xmf3Position.x, xmf3Position.y, xmf3Position.z);
}

void CGameObject::SetScale(float x, float y, float z)
{
	XMMATRIX mtxScale = XMMatrixScaling(x, y, z);
	m_xmf4x4ToParent = Matrix4x4::Multiply(mtxScale, m_xmf4x4ToParent);

	UpdateTransform(NULL);
}

XMFLOAT3 CGameObject::GetPosition()
{
	return(XMFLOAT3(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43));
}

XMFLOAT3 CGameObject::GetLook()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._31, m_xmf4x4World._32, m_xmf4x4World._33)));
}

XMFLOAT3 CGameObject::GetUp()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._21, m_xmf4x4World._22, m_xmf4x4World._23)));
}

XMFLOAT3 CGameObject::GetRight()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._11, m_xmf4x4World._12, m_xmf4x4World._13)));
}

void CGameObject::SetLookVector(XMFLOAT3& xmf3Look)
{
	m_xmf4x4ToParent._31 = xmf3Look.x;
	m_xmf4x4ToParent._32 = xmf3Look.y;
	m_xmf4x4ToParent._33 = xmf3Look.z;

	UpdateTransform(NULL);
}

void CGameObject::SetRightVector(XMFLOAT3& xmf3Right)
{
	m_xmf4x4ToParent._11 = xmf3Right.x;
	m_xmf4x4ToParent._12 = xmf3Right.y;
	m_xmf4x4ToParent._13 = xmf3Right.z;

	UpdateTransform(NULL);

}

void CGameObject::SetUpVector(XMFLOAT3& xmf3Up)
{
	m_xmf4x4ToParent._21 = xmf3Up.x;
	m_xmf4x4ToParent._22 = xmf3Up.y;
	m_xmf4x4ToParent._23 = xmf3Up.z;

	UpdateTransform(NULL);
}

void CGameObject::MoveStrafe(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Right = GetRight();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Right, fDistance);
	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::MoveUp(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Up = GetUp();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Up, fDistance);
	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::MoveForward(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Look = GetLook();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Look, fDistance);
	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::Rotate(float fPitch, float fYaw, float fRoll)
{
	XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(fPitch), XMConvertToRadians(fYaw), XMConvertToRadians(fRoll));
	m_xmf4x4ToParent = Matrix4x4::Multiply(mtxRotate, m_xmf4x4ToParent);

	UpdateTransform(NULL);
}

void CGameObject::Rotate(XMFLOAT3 *pxmf3Axis, float fAngle)
{
	XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(pxmf3Axis), XMConvertToRadians(fAngle));
	m_xmf4x4ToParent = Matrix4x4::Multiply(mtxRotate, m_xmf4x4ToParent);

	UpdateTransform(NULL);
}

void CGameObject::Rotate(XMFLOAT4 *pxmf4Quaternion)
{
	XMMATRIX mtxRotate = XMMatrixRotationQuaternion(XMLoadFloat4(pxmf4Quaternion));
	m_xmf4x4ToParent = Matrix4x4::Multiply(mtxRotate, m_xmf4x4ToParent);

	UpdateTransform(NULL);
}



//#define _WITH_DEBUG_FRAME_HIERARCHY

CTexture *CGameObject::FindReplicatedTexture(_TCHAR *pstrTextureName)
{
	for (int i = 0; i < m_nMaterials; i++)
	{
		if (m_ppMaterials[i])
		{
			for (int j = 0; j < m_ppMaterials[i]->m_nTextures; j++)
			{
				if (m_ppMaterials[i]->m_ppTextures[j])
				{
					if (!_tcsncmp(m_ppMaterials[i]->m_ppstrTextureNames[j], pstrTextureName, _tcslen(pstrTextureName))) 
						return(m_ppMaterials[i]->m_ppTextures[j]);
				}
			}
		}
	}
	CTexture *pTexture = NULL;
	if (m_pSibling) 
		if (pTexture = m_pSibling->FindReplicatedTexture(pstrTextureName)) 
			return(pTexture);
	if (m_pChild) 
		if (pTexture = m_pChild->FindReplicatedTexture(pstrTextureName)) 
			return(pTexture);

	return(NULL);
}

void CGameObject::LoadMaterialsFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, 
	CGameObject* pParent, FILE* pInFile, CShader* pShader, string type)
{
	char pstrToken[64] = { '\0' };

	int nMaterial = 0;
	BYTE nStrLength = 0;

	UINT nReads = (UINT)::fread(&m_nMaterials, sizeof(int), 1, pInFile);

	m_ppMaterials = new CMaterial*[m_nMaterials];
	for (int i = 0; i < m_nMaterials; i++)
		m_ppMaterials[i] = NULL;
		
	CMaterial* pMaterial = NULL;

	for ( ; ; )
	{
		nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
		nReads = (UINT)::fread(pstrToken, sizeof(char), nStrLength, pInFile); 
		pstrToken[nStrLength] = '\0';

		if (!strcmp(pstrToken, "<Material>:"))
		{
			nReads = (UINT)::fread(&nMaterial, sizeof(int), 1, pInFile);

			pMaterial = new CMaterial(7); //0:Albedo, 1:Specular, 2:Metallic, 3:Normal, 4:Emission, 5:DetailAlbedo, 6:DetailNormal

			if (!pShader)
			{
				UINT nMeshType = GetMeshType();
				if (nMeshType & VERTEXT_NORMAL_TANGENT_TEXTURE)
				{
					if (nMeshType & VERTEXT_BONE_INDEX_WEIGHT)
					{
						pMaterial->SetSkinnedAnimationShader();
					}
					else
					{
						pMaterial->SetStandardShader();
					}
				}
			}
			SetMaterial(nMaterial, pMaterial);
		}
		else if (!strcmp(pstrToken, "<AlbedoColor>:"))
		{
			nReads = (UINT)::fread(&(pMaterial->m_xmf4AlbedoColor), sizeof(float), 4, pInFile);
		}
		else if (!strcmp(pstrToken, "<EmissiveColor>:"))
		{
			nReads = (UINT)::fread(&(pMaterial->m_xmf4EmissiveColor), sizeof(float), 4, pInFile);
		}
		else if (!strcmp(pstrToken, "<SpecularColor>:"))
		{
			nReads = (UINT)::fread(&(pMaterial->m_xmf4SpecularColor), sizeof(float), 4, pInFile);
		}
		else if (!strcmp(pstrToken, "<Glossiness>:"))
		{
			nReads = (UINT)::fread(&(pMaterial->m_fGlossiness), sizeof(float), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<Smoothness>:"))
		{
			nReads = (UINT)::fread(&(pMaterial->m_fSmoothness), sizeof(float), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<Metallic>:"))
		{
			nReads = (UINT)::fread(&(pMaterial->m_fSpecularHighlight), sizeof(float), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<SpecularHighlight>:"))
		{
			nReads = (UINT)::fread(&(pMaterial->m_fMetallic), sizeof(float), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<GlossyReflection>:"))
		{
			nReads = (UINT)::fread(&(pMaterial->m_fGlossyReflection), sizeof(float), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<AlbedoMap>:"))
		{
			pMaterial->LoadTextureFromFile(pd3dDevice, pd3dCommandList, MATERIAL_ALBEDO_MAP, 3, pMaterial->m_ppstrTextureNames[0], &(pMaterial->m_ppTextures[0]), pParent, pInFile, pShader);
		}
		else if (!strcmp(pstrToken, "<SpecularMap>:"))
		{
			m_ppMaterials[nMaterial]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, MATERIAL_SPECULAR_MAP, 4, pMaterial->m_ppstrTextureNames[1], &(pMaterial->m_ppTextures[1]), pParent, pInFile, pShader);
		}
		else if (!strcmp(pstrToken, "<NormalMap>:"))
		{
			m_ppMaterials[nMaterial]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, MATERIAL_NORMAL_MAP, 5, pMaterial->m_ppstrTextureNames[2], &(pMaterial->m_ppTextures[2]), pParent, pInFile, pShader);
		}
		else if (!strcmp(pstrToken, "<MetallicMap>:"))
		{
			m_ppMaterials[nMaterial]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, MATERIAL_METALLIC_MAP, 6, pMaterial->m_ppstrTextureNames[3], &(pMaterial->m_ppTextures[3]), pParent, pInFile, pShader);
		}
		else if (!strcmp(pstrToken, "<EmissionMap>:"))
		{
			m_ppMaterials[nMaterial]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, MATERIAL_EMISSION_MAP, 7, pMaterial->m_ppstrTextureNames[4], &(pMaterial->m_ppTextures[4]), pParent, pInFile, pShader);
		}
		else if (!strcmp(pstrToken, "<DetailAlbedoMap>:"))
		{
			m_ppMaterials[nMaterial]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, MATERIAL_DETAIL_ALBEDO_MAP, 8, pMaterial->m_ppstrTextureNames[5], &(pMaterial->m_ppTextures[5]), pParent, pInFile, pShader);
		}
		else if (!strcmp(pstrToken, "<DetailNormalMap>:"))
		{
			m_ppMaterials[nMaterial]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, MATERIAL_DETAIL_NORMAL_MAP, 9, pMaterial->m_ppstrTextureNames[6], &(pMaterial->m_ppTextures[6]), pParent, pInFile, pShader);
		}
		else if (!strcmp(pstrToken, "</Materials>"))
		{
			break;
		}
	}

	for (int i = 0; i < m_nMaterials; ++i)
		m_ppMaterials[i]->CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

CGameObject* CGameObject::LoadFrameHierarchyFromFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, 
	CGameObject *pParent, FILE *pInFile, CShader *pShader, int *pnSkinnedMeshes, string type)
{
	char pstrToken[64] = { '\0' };

	BYTE nStrLength = 0;
	UINT nReads = 0;

	int nFrame = 0, nTextures = 0;

	CGameObject* pGameObject = NULL;

	for ( ; ; )
	{
		nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
		nReads = (UINT)::fread(pstrToken, sizeof(char), nStrLength, pInFile);
		pstrToken[nStrLength] = '\0';

		if (!strcmp(pstrToken, "<Frame>:"))
		{
			pGameObject = new CGameObject();

			nReads = (UINT)::fread(&nFrame, sizeof(int), 1, pInFile);
			nReads = (UINT)::fread(&nTextures, sizeof(int), 1, pInFile);

			nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
			nReads = (UINT)::fread(pGameObject->m_pstrFrameName, sizeof(char), nStrLength, pInFile);
			pGameObject->m_pstrFrameName[nStrLength] = '\0';

			if (!strcmp(pGameObject->m_pstrFrameName, "Lamp"))
			{
				pGameObject->m_pLampParticle = new CLampParticle(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
				pGameObject->SetChild(pGameObject->m_pLampParticle, true);
			}
		}
		else if (!strcmp(pstrToken, "<Transform>:"))
		{
			XMFLOAT3 xmf3Position, xmf3Rotation, xmf3Scale;
			XMFLOAT4 xmf4Rotation;
			nReads = (UINT)::fread(&xmf3Position, sizeof(float), 3, pInFile);
			nReads = (UINT)::fread(&xmf3Rotation, sizeof(float), 3, pInFile); //Euler Angle
			nReads = (UINT)::fread(&xmf3Scale, sizeof(float), 3, pInFile);
			nReads = (UINT)::fread(&xmf4Rotation, sizeof(float), 4, pInFile); //Quaternion

		}
		else if (!strcmp(pstrToken, "<TransformMatrix>:"))
		{
			nReads = (UINT)::fread(&pGameObject->m_xmf4x4ToParent, sizeof(float), 16, pInFile);
		}
		else if (!strcmp(pstrToken, "<Mesh>:"))
		{
			CStandardMesh* pMesh = new CStandardMesh(pd3dDevice, pd3dCommandList);
			pMesh->LoadMeshFromFile(pd3dDevice, pd3dCommandList, pInFile);

			pGameObject->SetMesh(pMesh);

			// 각 바운드 크기 셋
			pGameObject->SetOOBB(pMesh->getBoundCenter(), pMesh->getBoundExtent(), XMFLOAT4(0, 0, 0, 1));
		}
		else if (!strcmp(pstrToken, "<SkinningInfo>:"))
		{
			if (pnSkinnedMeshes) 
				(*pnSkinnedMeshes)++;

			CSkinnedMesh *pSkinnedMesh = new CSkinnedMesh(pd3dDevice, pd3dCommandList);
			pSkinnedMesh->CreateShaderVariables(pd3dDevice, pd3dCommandList);

			pSkinnedMesh->LoadSkinInfoFromFile(pd3dDevice, pd3dCommandList, pInFile);

			nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
			nReads = (UINT)::fread(pstrToken, sizeof(char), nStrLength, pInFile); //<Mesh>:
			pSkinnedMesh->LoadMeshFromFile(pd3dDevice, pd3dCommandList, pInFile);

			pGameObject->SetMesh(pSkinnedMesh);
		}
		else if (!strcmp(pstrToken, "<Materials>:"))
		{
			pGameObject->LoadMaterialsFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, pParent, pInFile, pShader, type);
		}
		else if (!strcmp(pstrToken, "<Children>:"))
		{
			int nChilds = 0;
			nReads = (UINT)::fread(&nChilds, sizeof(int), 1, pInFile);
			if (nChilds > 0)
			{
				for (int i = 0; i < nChilds; i++)
				{
					CGameObject *pChild = CGameObject::LoadFrameHierarchyFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, 
						pGameObject, pInFile, pShader, pnSkinnedMeshes, type);
					if (pChild) pGameObject->SetChild(pChild);
#ifdef _WITH_DEBUG_FRAME_HIERARCHY
					TCHAR pstrDebug[256] = { 0 };
					_stprintf_s(pstrDebug, 256, "(Frame: %p) (Parent: %p)\n"), pChild, pGameObject);
					OutputDebugString(pstrDebug);
#endif
				}
			}
		}
		else if (!strcmp(pstrToken, "</Frame>"))
		{
			break;
		}
	}

	return(pGameObject);
}

void CGameObject::PrintFrameInfo(CGameObject *pGameObject, CGameObject *pParent)
{
	TCHAR pstrDebug[256] = { 0 };

	_stprintf_s(pstrDebug, 256, _T("(Frame: %p) (Parent: %p)\n"), pGameObject, pParent);
	OutputDebugString(pstrDebug);

	if (pGameObject->m_pSibling) 
		CGameObject::PrintFrameInfo(pGameObject->m_pSibling, pParent);
	if (pGameObject->m_pChild) 
		CGameObject::PrintFrameInfo(pGameObject->m_pChild, pGameObject);
}

CAnimationSets *CGameObject::LoadAnimationFromFile(FILE *pInFile, CGameObject *pRootFrame)
{
	char pstrToken[64] = { '\0' };

	BYTE nStrLength = 0;
	UINT nReads = 0;

	CAnimationSets *pAnimationSets = new CAnimationSets();

	for ( ; ; )
	{
		nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
		nReads = (UINT)::fread(pstrToken, sizeof(char), nStrLength, pInFile);
		pstrToken[nStrLength] = '\0';

		if (!strcmp(pstrToken, "<AnimationSets>:"))
		{
			nReads = (UINT)::fread(&pAnimationSets->m_nAnimationSets, sizeof(int), 1, pInFile);

			pAnimationSets->m_pAnimationSets = new CAnimationSet[pAnimationSets->m_nAnimationSets];
		}
		else if (!strcmp(pstrToken, "<FrameNames>:"))
		{
			nReads = (UINT)::fread(&pAnimationSets->m_nAnimationFrames, sizeof(int), 1, pInFile);
			pAnimationSets->m_ppAnimationFrameCaches = new CGameObject*[pAnimationSets->m_nAnimationFrames];

			for (int i = 0; i < pAnimationSets->m_nAnimationFrames; i++)
			{
				nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
				nReads = (UINT)::fread(pstrToken, sizeof(char), nStrLength, pInFile);
				pstrToken[nStrLength] = '\0';

				pAnimationSets->m_ppAnimationFrameCaches[i] = pRootFrame->FindFrame(pstrToken);

#ifdef _WITH_DEBUG_SKINNING_BONE
				TCHAR pstrDebug[256] = { 0 };
				TCHAR pwstrAnimationBoneName[64] = { 0 };
				TCHAR pwstrBoneCacheName[64] = { 0 };
				size_t nConverted = 0;
				mbstowcs_s(&nConverted, pwstrAnimationBoneName, 64, pstrToken, _TRUNCATE);
				mbstowcs_s(&nConverted, pwstrBoneCacheName, 64, pAnimationSets->m_ppAnimationFrameCaches[i]->m_pstrFrameName, _TRUNCATE);
				_stprintf_s(pstrDebug, 256, _T("AnimationBoneFrame:: Cache(%s) AnimationBone(%s)\n"), pwstrBoneCacheName, pwstrAnimationBoneName);
				OutputDebugString(pstrDebug);
#endif
			}
		}
		else if (!strcmp(pstrToken, "<AnimationSet>:"))
		{
			int nAnimationSet = 0;
			nReads = (UINT)::fread(&nAnimationSet, sizeof(int), 1, pInFile);
			CAnimationSet *pAnimationSet = &pAnimationSets->m_pAnimationSets[nAnimationSet];

			nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
			nReads = (UINT)::fread(pAnimationSet->m_pstrName, sizeof(char), nStrLength, pInFile);
			pAnimationSet->m_pstrName[nStrLength] = '\0';

			// 애니메이션이 안되는 문제해결해야댐
			if (!strcmp(pAnimationSet->m_pstrName, "ATK3") || !strcmp(pAnimationSet->m_pstrName, "Digging") 
				|| !strcmp(pAnimationSet->m_pstrName,"Jump") ||!strcmp(pAnimationSet->m_pstrName, "RaiseHand")
				|| !strcmp(pAnimationSet->m_pstrName,"Die2")
				) 

			{
				pAnimationSet->m_nType = ANIMATION_TYPE_ONCE;
			}

			nReads = (UINT)::fread(&pAnimationSet->m_fLength, sizeof(float), 1, pInFile);
			nReads = (UINT)::fread(&pAnimationSet->m_nFramesPerSecond, sizeof(int), 1, pInFile);

			nReads = (UINT)::fread(&pAnimationSet->m_nKeyFrameTransforms, sizeof(int), 1, pInFile);
			pAnimationSet->m_pfKeyFrameTransformTimes = new float[pAnimationSet->m_nKeyFrameTransforms];
			pAnimationSet->m_ppxmf4x4KeyFrameTransforms = new XMFLOAT4X4*[pAnimationSet->m_nKeyFrameTransforms];
			for (int i = 0; i < pAnimationSet->m_nKeyFrameTransforms; i++) pAnimationSet->m_ppxmf4x4KeyFrameTransforms[i] = new XMFLOAT4X4[pAnimationSets->m_nAnimationFrames];

			for (int i = 0; i < pAnimationSet->m_nKeyFrameTransforms; i++)
			{
				nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
				nReads = (UINT)::fread(pstrToken, sizeof(char), nStrLength, pInFile);
				pstrToken[nStrLength] = '\0';

				if (!strcmp(pstrToken, "<Transforms>:"))
				{
					int nKeyFrame = 0;
					nReads = (UINT)::fread(&nKeyFrame, sizeof(int), 1, pInFile); //i

					nReads = (UINT)::fread(&pAnimationSet->m_pfKeyFrameTransformTimes[i], sizeof(float), 1, pInFile);
					nReads = (UINT)::fread(pAnimationSet->m_ppxmf4x4KeyFrameTransforms[i], sizeof(float), 16 * pAnimationSets->m_nAnimationFrames, pInFile);
				}
			}
#ifdef _WITH_ANIMATION_SRT
			nReads = (UINT)::fread(&pAnimationSet->m_nKeyFrameTranslations, sizeof(int), 1, pInFile);
			pAnimationSet->m_pfKeyFrameTranslationTimes = new float[pAnimationSet->m_nKeyFrameTranslations];
			pAnimationSet->m_ppxmf3KeyFrameTranslations = new XMFLOAT3*[pAnimationSet->m_nKeyFrameTranslations];
			for (int i = 0; i < pAnimationSet->m_nKeyFrameTranslations; i++) pAnimationSet->m_ppxmf3KeyFrameTranslations[i] = new XMFLOAT3[pAnimationSets->m_nAnimationFrames];
#endif
		}
		else if (!strcmp(pstrToken, "</AnimationSets>"))
		{
			break;
		}
	}

	return(pAnimationSets);
}

void CGameObject::CacheSkinningBoneFrames(CGameObject *pRootFrame)
{
	if (m_pMesh && (m_pMesh->GetType() & VERTEXT_BONE_INDEX_WEIGHT))
	{
		CSkinnedMesh *pSkinnedMesh = (CSkinnedMesh *)m_pMesh;
		for (int i = 0; i < pSkinnedMesh->m_nSkinningBones; i++)
		{
			pSkinnedMesh->m_ppSkinningBoneFrameCaches[i] = pRootFrame->FindFrame(pSkinnedMesh->m_ppstrSkinningBoneNames[i]);
			
#ifdef _WITH_DEBUG_SKINNING_BONE
			TCHAR pstrDebug[256] = { 0 };
			TCHAR pwstrBoneCacheName[64] = { 0 };
			TCHAR pwstrSkinningBoneName[64] = { 0 };
			size_t nConverted = 0;
			mbstowcs_s(&nConverted, pwstrBoneCacheName, 64, pSkinnedMesh->m_ppSkinningBoneFrameCaches[i]->m_pstrFrameName, _TRUNCATE);
			mbstowcs_s(&nConverted, pwstrSkinningBoneName, 64, pSkinnedMesh->m_ppstrSkinningBoneNames[i], _TRUNCATE);
			_stprintf_s(pstrDebug, 256, _T("SkinningBoneFrame:: Cache(%s) Bone(%s)\n"), pwstrBoneCacheName, pwstrSkinningBoneName);
			OutputDebugString(pstrDebug);
#endif
		}
	}
	if (m_pSibling) 
		m_pSibling->CacheSkinningBoneFrames(pRootFrame);
	if (m_pChild) 
		m_pChild->CacheSkinningBoneFrames(pRootFrame);
}

void CGameObject::FindAndSetSkinnedMesh(int *pnSkinMesh, CSkinningBoneTransforms *pSkinningBoneTransforms)
{
	if (m_pMesh && (m_pMesh->GetType() & VERTEXT_BONE_INDEX_WEIGHT))
	{
		CSkinnedMesh *pSkinnedMesh = (CSkinnedMesh *)m_pMesh;
		pSkinningBoneTransforms->SetSkinnedMesh((*pnSkinMesh)++, pSkinnedMesh);
	}
	if (m_pSibling) 
		m_pSibling->FindAndSetSkinnedMesh(pnSkinMesh, pSkinningBoneTransforms);
	if (m_pChild) 
		m_pChild->FindAndSetSkinnedMesh(pnSkinMesh, pSkinningBoneTransforms);
}

CLoadedModelInfo* CGameObject::LoadGeometryAndAnimationFromFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, 
	char *pstrFileName, CShader *pShader, bool bHasAnimation, string type)
{
	FILE *pInFile = NULL;
	::fopen_s(&pInFile, pstrFileName, "rb");

	::rewind(pInFile);

	CLoadedModelInfo* pLoadedModel = new CLoadedModelInfo;
	pLoadedModel->m_pModelRootObject = CGameObject::LoadFrameHierarchyFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, 
		NULL, pInFile, pShader, &pLoadedModel->m_nSkinnedMeshes, type);

	if (bHasAnimation) 
		pLoadedModel->m_pAnimationSets = CGameObject::LoadAnimationFromFile(pInFile, pLoadedModel->m_pModelRootObject);
	
	pLoadedModel->m_pModelRootObject->CacheSkinningBoneFrames(pLoadedModel->m_pModelRootObject);

#ifdef _WITH_DEBUG_FRAME_HIERARCHY
	TCHAR pstrDebug[256] = { 0 };
	_stprintf_s(pstrDebug, 256, "Frame Hierarchy\n"));
	OutputDebugString(pstrDebug);

	CGameObject::PrintFrameInfo(pGameObject, NULL);
#endif

	return(pLoadedModel);
}

CCubeObject::CCubeObject(int nMaterial) : CGameObject(nMaterial)
{
}

CCubeObject::~CCubeObject()
{
}

void CCubeObject::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	OnPrepareRender();

	if (m_pMesh)
	{
		//if (!m_pSkinningBoneTransforms)
		//	UpdateShaderVariable(pd3dCommandList, &m_xmf4x4World);

		if (m_nMaterials > 0)
		{
			for (int i = 0; i < m_nMaterials; i++)
			{
				if (m_ppMaterials[i])
				{
					if (m_ppMaterials[i]->m_pShader)
						m_ppMaterials[i]->m_pShader->Render(pd3dCommandList, pCamera, GameObject);

					m_ppMaterials[i]->UpdateShaderVariables(pd3dCommandList);
				}
				m_pMesh->Render(pd3dCommandList, i);
			}
		}
	}
}



#include "../../../../Stdafx/Stdafx.h"
#include "SkinnedAnimationObjectShader.h"
#include "../../../../ResourceManager/ResourceManager.h"
#include "../../../../GameObject/GameObject.h"
#include "../../../../GameObject/Terrain/Terrain.h"
#include "../../../../GameObject/EvilBear/EvilBear.h"
#include "../../../../GameObject/Shadow/Shadow.h"

CSkinnedAnimationObjectShader::CSkinnedAnimationObjectShader()
{
}

CSkinnedAnimationObjectShader::~CSkinnedAnimationObjectShader()
{
}

void CSkinnedAnimationObjectShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature,
	const map<string, CLoadedModelInfo*>& ModelMap, const map<string, Bounds*>& Context,const int& nPlayerCount, void* pContext)
{
	CTerrain* pTerrain = (CTerrain *)pContext;
	m_nObjects = nPlayerCount;

	m_ppObjects = new CGameObject*[m_nObjects];
	
	auto Model = ModelMap.find("EvilBear");
	if (Model != ModelMap.end())
	{
		m_ppObjects[0] = new CEvilBear(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, CGameObject::MATERIALTYPE::PINK);
		m_ppObjects[0]->SetChild((*Model).second->m_pModelRootObject, true);
		m_ppObjects[0]->m_pAnimationController = new CAnimationController(1, (*Model).second->m_pAnimationSets);
		// 0번 트랙에 0번 애니메이션을 Set
		m_ppObjects[0]->m_pAnimationController->SetTrackAnimationSet(0, m_ppObjects[0]->m_pAnimationController->RUNFAST);
		// 1번 트랙에 1번 애니메이션을 Set
		//m_ppObjects[0]->m_pAnimationController->SetTrackAnimationSet(1, 1);
		// 0번 트랙에 가중치를 80%
	//	m_ppObjects[0]->m_pAnimationController->SetTrackWeight(0, 0.8f);
		// 1번 트랙에 가중치를 20% 
		//m_ppObjects[0]->m_pAnimationController->SetTrackWeight(1, 0.2f);
		m_ppObjects[0]->m_pSkinningBoneTransforms = new CSkinningBoneTransforms(pd3dDevice, pd3dCommandList, (*Model).second);

		m_ppObjects[1] = new CEvilBear(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, CGameObject::MATERIALTYPE::BLACK);
		m_ppObjects[1]->SetChild((*Model).second->m_pModelRootObject, true);
		m_ppObjects[1]->m_pAnimationController = new CAnimationController(1, (*Model).second->m_pAnimationSets);
		//m_ppObjects[1]->m_pAnimationController->SetTrackAnimationSet(1, 2);
		m_ppObjects[1]->m_pAnimationController->SetTrackAnimationSet(0, m_ppObjects[1]->m_pAnimationController->VICTORY);
		//m_ppObjects[1]->m_pAnimationController->SetTrackSpeed(0, 0.8f);
		//m_ppObjects[1]->m_pAnimationController->SetTrackWeight(0, 0.9);
		//m_ppObjects[1]->m_pAnimationController->SetTrackWeight(1, 0.1);
		// 애니메이션의 속도를 0.25로 주어서 느리게 애니메이션 동작을 하도록 Set
		//m_ppObjects[1]->m_pAnimationController->SetTrackSpeed(0, 0.25f);
		m_ppObjects[1]->m_pSkinningBoneTransforms = new CSkinningBoneTransforms(pd3dDevice, pd3dCommandList, (*Model).second);

		m_ppObjects[2] = new CEvilBear(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, CGameObject::MATERIALTYPE::BROWN);
		m_ppObjects[2]->SetChild((*Model).second->m_pModelRootObject, true);
		m_ppObjects[2]->m_pAnimationController = new CAnimationController(1, (*Model).second->m_pAnimationSets);
		m_ppObjects[2]->m_pAnimationController->SetTrackAnimationSet(0, m_ppObjects[2]->m_pAnimationController->DIE);
		// 애니메이션의 시작위치를 다르게 준다. 그러면 같은 동작이더라도 다르게 애니메이션함
		//m_ppObjects[2]->m_pAnimationController->SetTrackPosition(0, 0.95f);
		m_ppObjects[2]->m_pSkinningBoneTransforms = new CSkinningBoneTransforms(pd3dDevice, pd3dCommandList, (*Model).second);

		m_ppObjects[3] = new CEvilBear(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, CGameObject::MATERIALTYPE::BLUE);
		m_ppObjects[3]->SetChild((*Model).second->m_pModelRootObject, true);
		m_ppObjects[3]->m_pAnimationController = new CAnimationController(1, (*Model).second->m_pAnimationSets);
		m_ppObjects[3]->m_pAnimationController->SetTrackAnimationSet(0, m_ppObjects[3]->m_pAnimationController->RAISEHAND);
		m_ppObjects[3]->m_pSkinningBoneTransforms = new CSkinningBoneTransforms(pd3dDevice, pd3dCommandList, (*Model).second);

		m_ppObjects[4] = new CEvilBear(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, CGameObject::MATERIALTYPE::ICEMAT);
		m_ppObjects[4]->SetChild((*Model).second->m_pModelRootObject, true);
		m_ppObjects[4]->m_pAnimationController = new CAnimationController(1, (*Model).second->m_pAnimationSets);
		m_ppObjects[4]->m_pAnimationController->SetTrackAnimationSet(0, m_ppObjects[4]->m_pAnimationController->ATTACK);
		m_ppObjects[4]->m_pSkinningBoneTransforms = new CSkinningBoneTransforms(pd3dDevice, pd3dCommandList, (*Model).second);
	}

	CAnimationCallbackHandler* pRunAnimationCallback = new CSoundCallbackHandler();
	CAnimationCallbackHandler* pRaiseHandAnimationCallback = new CSoundCallbackHandler();

	for(UINT i=0;i<m_nObjects;i++)		//플레이어 수만큼 사운드 효과 설정해준다
	{
		//m_ppObjects[i]->m_pAnimationController->SetCallbackKeys(m_ppObjects[i]->m_pAnimationController->RUNFAST, 2);
		//m_ppObjects[i]->m_pAnimationController->SetCallbackKey(m_ppObjects[i]->m_pAnimationController->RUNFAST, 0, 0.3f, (void*)CEvilBear::MUSIC_ENUM::FOOTSTEP/*MAKEINTRESOURCE(IDR_WAVE2)*/);
		//m_ppObjects[i]->m_pAnimationController->SetCallbackKey(m_ppObjects[i]->m_pAnimationController->RUNFAST, 1, 0.6f, (void*)CEvilBear::MUSIC_ENUM::FOOTSTEP/*MAKEINTRESOURCE(IDR_WAVE2)*/);
		//m_ppObjects[i]->m_pAnimationController->SetAnimationCallbackHandler(m_ppObjects[i]->m_pAnimationController->RUNFAST, pAnimationCallback,
		//	dynamic_cast<CEvilBear*>(m_ppObjects[i])->GetSoundData());
		auto map = dynamic_cast<CEvilBear*>(m_ppObjects[i])->m_mapMusicList;
		
		auto iter = map.find(CEvilBear::MUSIC_ENUM::FOOTSTEP);
		
		if(iter != map.end())
		{
			m_ppObjects[i]->m_pAnimationController->SetCallbackKeys(m_ppObjects[i]->m_pAnimationController->RUNFAST, 2);
			m_ppObjects[i]->m_pAnimationController->SetCallbackKey(m_ppObjects[i]->m_pAnimationController->RUNFAST, 0, 0.3f,(void*)CEvilBear::MUSIC_ENUM::FOOTSTEP/*MAKEINTRESOURCE(IDR_WAVE2)*/);
			m_ppObjects[i]->m_pAnimationController->SetCallbackKey(m_ppObjects[i]->m_pAnimationController->RUNFAST, 1, 0.6f, (void*)CEvilBear::MUSIC_ENUM::FOOTSTEP/*MAKEINTRESOURCE(IDR_WAVE2)*/);
			m_ppObjects[i]->m_pAnimationController->SetAnimationCallbackHandler(m_ppObjects[i]->m_pAnimationController->RUNFAST, pRunAnimationCallback,
			dynamic_cast<CEvilBear*>(m_ppObjects[i])->GetSoundData());
		}

		iter = map.find(CEvilBear::MUSIC_ENUM::USETIMER);
		
		if(iter != map.end())
		{
			m_ppObjects[i]->m_pAnimationController->SetCallbackKeys(m_ppObjects[i]->m_pAnimationController->RAISEHAND,1);
			m_ppObjects[i]->m_pAnimationController->SetCallbackKey(m_ppObjects[i]->m_pAnimationController->RAISEHAND, 0, 0.3f,(void*)CEvilBear::MUSIC_ENUM::USETIMER/*MAKEINTRESOURCE(IDR_WAVE2)*/);
			m_ppObjects[i]->m_pAnimationController->SetAnimationCallbackHandler(m_ppObjects[i]->m_pAnimationController->RAISEHAND, pRaiseHandAnimationCallback,
			dynamic_cast<CEvilBear*>(m_ppObjects[i])->GetSoundData());
		}
	}

	XMFLOAT3 Position;
	for (int i = 0; i < m_nObjects; ++i)
	{
		Position.x = Random(0.f, 500.f);
		Position.z = Random(0.f, 300.f);
		Position.y = pTerrain->GetHeight(Position.x, Position.z);
		m_ppObjects[i]->SetPosition(Position);
		m_ppObjects[i]->SetScale(10, 10, 10);

		m_ppObjects[i]->setID("<EvilBear>");
		auto iter = Context.find(m_ppObjects[i]->getID());
		if (iter != Context.end())
			m_ppObjects[i]->SetOOBB((*iter).second->m_xmf3Center, (*iter).second->m_xmf3Extent, XMFLOAT4(0, 0, 0, 1));

		dynamic_cast<CEvilBear*>(m_ppObjects[i])->Initialize_Shadow((*Model).second, m_ppObjects[i]);
	}
}

void CSkinnedAnimationObjectShader::AnimateObjects(float fTimeElapsed, CCamera* pCamera,CPlayer* pPlayer)
{
	m_elapsedTime = fTimeElapsed;
}

void CSkinnedAnimationObjectShader::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState)
{
	for (int i = 0; i < m_nObjects; ++i)
	{
		if (m_ppObjects[i])
		{
			m_ppObjects[i]->Animate(m_elapsedTime);
			m_ppObjects[i]->UpdateTransform(NULL);
			m_ppObjects[i]->Render(pd3dCommandList, m_ppObjects[i]->GetIsHammer(), m_ppObjects[i]->GetIsBomb(), m_ppObjects[i]->GetBoolIce(), m_ppObjects[i]->GetMaterialID(), pCamera, nPipelineState);
		}
	}
}
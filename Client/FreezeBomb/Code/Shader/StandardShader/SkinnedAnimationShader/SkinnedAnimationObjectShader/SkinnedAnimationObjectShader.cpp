#include "../../../../Stdafx/Stdafx.h"
#include "SkinnedAnimationObjectShader.h"
#include "../../../../ResourceManager/ResourceManager.h"
#include "../../../../GameObject/GameObject.h"
#include "../../../../GameObject/Terrain/Terrain.h"
#include "../../../../GameObject/EvilBear/EvilBear.h"
#include "../../../../GameObject/Shadow/Shadow.h"
#include "../../../../FrameTransform/FrameTransform.h"
#include "../../../../Network/Network.h"
#include "../../../../SoundSystem/SoundSystem.h"
#include "../../../../Scene/LobbyScene/LobbyScene.h"

CSkinnedAnimationObjectShader::CSkinnedAnimationObjectShader()	:m_userID{ 0 }	
{
}

CSkinnedAnimationObjectShader::~CSkinnedAnimationObjectShader()
{
	MaterialClear();
	//m_mapMaterial.clear();
	//m_vMaterial.clear();
}

void CSkinnedAnimationObjectShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature,
	const map<string, CLoadedModelInfo*>& ModelMap, const map<string, Bounds*>& Context, const int& nPlayerCount, void* pContext)
{
	CTerrain* pTerrain = (CTerrain *)pContext;
	m_nObjects = nPlayerCount-1;

	m_ppObjects = new CGameObject*[m_nObjects];
	
	auto Model = ModelMap.find("EvilBear");
	if (Model != ModelMap.end())
	{
		m_ppObjects[0] = new CEvilBear(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, CGameObject::MATERIALTYPE::PINK);
		m_ppObjects[0]->SetChild((*Model).second->m_pModelRootObject, true);
		m_ppObjects[0]->m_pAnimationController = new CAnimationController(1, (*Model).second->m_pAnimationSets);
		// 0번 트랙에 0번 애니메이션을 Set
		m_ppObjects[0]->m_pAnimationController->SetTrackAnimationSet(0, m_ppObjects[0]->m_pAnimationController->IDLE);
		m_ppObjects[0]->m_pSkinningBoneTransforms = new CSkinningBoneTransforms(pd3dDevice, pd3dCommandList, (*Model).second);
		//dynamic_cast<CEvilBear*>(m_ppObjects[0])->SetPlayerName(L"핑크");

		m_ppObjects[1] = new CEvilBear(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, CGameObject::MATERIALTYPE::BROWN);
		m_ppObjects[1]->SetChild((*Model).second->m_pModelRootObject, true);
		m_ppObjects[1]->m_pAnimationController = new CAnimationController(1, (*Model).second->m_pAnimationSets);
		m_ppObjects[1]->m_pAnimationController->SetTrackAnimationSet(0, m_ppObjects[1]->m_pAnimationController->USEGOLDHAMMER);
		//m_ppObjects[1]->SetIsLightEffect(true);
		m_ppObjects[1]->m_pSkinningBoneTransforms = new CSkinningBoneTransforms(pd3dDevice, pd3dCommandList, (*Model).second);
		//dynamic_cast<CEvilBear*>(m_ppObjects[1])->SetPlayerName(L"브라운");

		m_ppObjects[2] = new CEvilBear(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, CGameObject::MATERIALTYPE::WHITE);
		m_ppObjects[2]->SetChild((*Model).second->m_pModelRootObject, true);
		m_ppObjects[2]->m_pAnimationController = new CAnimationController(1, (*Model).second->m_pAnimationSets);
		m_ppObjects[2]->m_pAnimationController->SetTrackAnimationSet(0, m_ppObjects[2]->m_pAnimationController->ATTACK);
		m_ppObjects[2]->m_pSkinningBoneTransforms = new CSkinningBoneTransforms(pd3dDevice, pd3dCommandList, (*Model).second);
	//	dynamic_cast<CEvilBear*>(m_ppObjects[2])->SetPlayerName(L"화이트");

		m_ppObjects[3] = new CEvilBear(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, CGameObject::MATERIALTYPE::BLACK);
		m_ppObjects[3]->SetChild((*Model).second->m_pModelRootObject, true);
		m_ppObjects[3]->m_pAnimationController = new CAnimationController(1, (*Model).second->m_pAnimationSets);
		m_ppObjects[3]->m_pAnimationController->SetTrackAnimationSet(0, m_ppObjects[3]->m_pAnimationController->RAISEHAND);
		m_ppObjects[3]->m_pSkinningBoneTransforms = new CSkinningBoneTransforms(pd3dDevice, pd3dCommandList, (*Model).second);
		//dynamic_cast<CEvilBear*>(m_ppObjects[3])->SetPlayerName(L"블랙");

		m_ppObjects[4] = new CEvilBear(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, CGameObject::MATERIALTYPE::BLUE);
		m_ppObjects[4]->SetChild((*Model).second->m_pModelRootObject, true);
		m_ppObjects[4]->m_pAnimationController = new CAnimationController(1, (*Model).second->m_pAnimationSets);
		m_ppObjects[4]->m_pAnimationController->SetTrackAnimationSet(0, m_ppObjects[4]->m_pAnimationController->IDLE);
		m_ppObjects[4]->m_pSkinningBoneTransforms = new CSkinningBoneTransforms(pd3dDevice, pd3dCommandList, (*Model).second);
		//dynamic_cast<CEvilBear*>(m_ppObjects[4])->SetPlayerName(L"블루");

		/*m_ppObjects[5] = new CEvilBear(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, CGameObject::MATERIALTYPE::BLUE);
		m_ppObjects[5]->SetChild((*Model).second->m_pModelRootObject, true);
		m_ppObjects[5]->m_pAnimationController = new CAnimationController(1, (*Model).second->m_pAnimationSets);
		m_ppObjects[5]->m_pAnimationController->SetTrackAnimationSet(0, m_ppObjects[5]->m_pAnimationController->IDLE);
		m_ppObjects[5]->m_pSkinningBoneTransforms = new CSkinningBoneTransforms(pd3dDevice, pd3dCommandList, (*Model).second);*/

		for (int i = 0; i < m_nObjects; ++i)
		{
			CAnimationCallbackHandler* pRunAnimationCallback = new CSoundCallbackHandler;
			pRunAnimationCallback->SetAdditianalData(i);
			CAnimationCallbackHandler* pBackRunAnimationCallback = new CSoundCallbackHandler;
			pBackRunAnimationCallback->SetAdditianalData(i);
			CAnimationCallbackHandler* pRaiseHandAnimationCallback = new CSoundCallbackHandler;
			pRaiseHandAnimationCallback->SetAdditianalData(i);
			CAnimationCallbackHandler* pDieAnimationCallback = new CSoundCallbackHandler;
			pDieAnimationCallback->SetAdditianalData(i);
			CAnimationCallbackHandler* pAttackAnimationCallback = new CSoundCallbackHandler;
			pAttackAnimationCallback->SetAdditianalData(i);
			CAnimationCallbackHandler* pUseGoldHammerCallback = new CSoundCallbackHandler;
			pUseGoldHammerCallback->SetAdditianalData(i);

			// 애니메이션 Transform을 각자 갖고있게 하기위해
			m_ppObjects[i]->m_pSkinningBoneTransforms = new CSkinningBoneTransforms(pd3dDevice, pd3dCommandList, (*Model).second);
			// 메쉬의 월드행렬을 각자 갖고있게 하기위해
			m_ppObjects[i]->m_pFrameTransform = new CFrameTransform(pd3dDevice, pd3dCommandList, (*Model).second);

			m_ppObjects[i]->m_pAnimationController->SetCallbackKeys(m_ppObjects[i]->m_pAnimationController->RUNFAST, 2);
			m_ppObjects[i]->m_pAnimationController->SetCallbackKey(m_ppObjects[i]->m_pAnimationController->RUNFAST, 0, 0.1f, (void*)CSoundSystem::SOUND_TYPE::RUN1);
			m_ppObjects[i]->m_pAnimationController->SetCallbackKey(m_ppObjects[i]->m_pAnimationController->RUNFAST, 1, 0.4f, (void*)CSoundSystem::SOUND_TYPE::RUN2);
			m_ppObjects[i]->m_pAnimationController->SetAnimationCallbackHandler(m_ppObjects[i]->m_pAnimationController->RUNFAST, pRunAnimationCallback);

			m_ppObjects[i]->m_pAnimationController->SetCallbackKeys(m_ppObjects[i]->m_pAnimationController->RUNBACKWARD, 2);
			m_ppObjects[i]->m_pAnimationController->SetCallbackKey(m_ppObjects[i]->m_pAnimationController->RUNBACKWARD, 0, 0.1f, (void*)CSoundSystem::SOUND_TYPE::RUN1);
			m_ppObjects[i]->m_pAnimationController->SetCallbackKey(m_ppObjects[i]->m_pAnimationController->RUNBACKWARD, 1, 0.25f, (void*)CSoundSystem::SOUND_TYPE::RUN2);
			m_ppObjects[i]->m_pAnimationController->SetAnimationCallbackHandler(m_ppObjects[i]->m_pAnimationController->RUNBACKWARD, pBackRunAnimationCallback);

			m_ppObjects[i]->m_pAnimationController->SetCallbackKeys(m_ppObjects[i]->m_pAnimationController->RAISEHAND, 1);
			m_ppObjects[i]->m_pAnimationController->SetCallbackKey(m_ppObjects[i]->m_pAnimationController->RAISEHAND, 0, 0.3f, (void*)CSoundSystem::SOUND_TYPE::GOLDTIMER_EFFECT);
			m_ppObjects[i]->m_pAnimationController->SetAnimationCallbackHandler(m_ppObjects[i]->m_pAnimationController->RAISEHAND, pRaiseHandAnimationCallback);

			m_ppObjects[i]->m_pAnimationController->SetCallbackKeys(m_ppObjects[i]->m_pAnimationController->DIE, 1);
			m_ppObjects[i]->m_pAnimationController->SetCallbackKey(m_ppObjects[i]->m_pAnimationController->DIE, 0, 0.1f, (void*)CSoundSystem::SOUND_TYPE::BOMBEXPLOSION_EFFECT);
			m_ppObjects[i]->m_pAnimationController->SetAnimationCallbackHandler(m_ppObjects[i]->m_pAnimationController->DIE, pDieAnimationCallback);

			m_ppObjects[i]->m_pAnimationController->SetCallbackKeys(m_ppObjects[i]->m_pAnimationController->ATTACK, 1);
			m_ppObjects[i]->m_pAnimationController->SetCallbackKey(m_ppObjects[i]->m_pAnimationController->ATTACK, 0, 0.2f, (void*)CSoundSystem::SOUND_TYPE::HAMMERSWING_EFFECT);
			m_ppObjects[i]->m_pAnimationController->SetAnimationCallbackHandler(m_ppObjects[i]->m_pAnimationController->ATTACK, pAttackAnimationCallback);

			m_ppObjects[i]->m_pAnimationController->SetCallbackKeys(m_ppObjects[i]->m_pAnimationController->USEGOLDHAMMER, 1);
			m_ppObjects[i]->m_pAnimationController->SetCallbackKey(m_ppObjects[i]->m_pAnimationController->USEGOLDHAMMER, 0, 0.3f, (void*)CSoundSystem::SOUND_TYPE::GOLDHAMMER_EFFECT);
			m_ppObjects[i]->m_pAnimationController->SetAnimationCallbackHandler(m_ppObjects[i]->m_pAnimationController->USEGOLDHAMMER, pUseGoldHammerCallback);
		}
	}

#ifndef _WITH_SERVER_
	// 임시로 플레이어 위치 고정
	int index = 0;
	m_ppObjects[index]->SetPosition(100, 0, 100);
	m_ppObjects[index++]->SetIsICE(true);

	m_ppObjects[index++]->SetPosition(58, 0, 180);
	m_ppObjects[index++]->SetPosition(192, 0, 230);
	m_ppObjects[index++]->SetPosition(386, 0, 206);
	
	m_ppObjects[index]->SetPosition(70, 0, 50);
	m_ppObjects[index]->SetIsICE(true);
#endif

	for (int i = 0; i < m_nObjects; ++i)
	{
		XMFLOAT3 scale = XMFLOAT3(10.0f, 10.0f, 10.0f);
		m_ppObjects[i]->SetScale(scale.x, scale.y, scale.z);

		m_ppObjects[i]->setID("EvilBear");
		auto iter = Context.find(m_ppObjects[i]->getID());
		if (iter != Context.end()) 
		{
#ifdef _WITH_SERVER_
			m_ppObjects[i]->SetOOBB((*iter).second->m_xmf3Center, Vector3::Multiply((*iter).second->m_xmf3Extent, XMFLOAT3(2.0f,2.0f,2.0f)), XMFLOAT4(0, 0, 0, 1));
#else
			m_ppObjects[i]->SetOOBB((*iter).second->m_xmf3Center, (*iter).second->m_xmf3Extent, XMFLOAT4(0, 0, 0, 1));
#endif
		}
		dynamic_cast<CEvilBear*>(m_ppObjects[i])->Initialize_Shadow((*Model).second, m_ppObjects[i]);
	}
}

void CSkinnedAnimationObjectShader::AnimateObjects(float fTimeElapsed, CCamera* pCamera,CPlayer* pPlayer)
{
	m_elapsedTime = fTimeElapsed;
}

void CSkinnedAnimationObjectShader::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState)
{
#ifdef _WITH_SERVER_
	for (auto enemyID : m_mapMaterial)
	{
		char matID = enemyID.second;
		char id = enemyID.first;

		
		dynamic_cast<CEvilBear*>(m_ppObjects[id])->Animate(m_elapsedTime);
		m_ppObjects[id]->UpdateTransform(NULL);
		dynamic_cast<CEvilBear*>(m_ppObjects[id])->Render(pd3dCommandList, pCamera, matID ,nPipelineState);		
	}
#else
	for (int i = 0; i < m_nObjects; ++i)
	{
		if (m_ppObjects[i])
		{
			m_ppObjects[i]->Animate(m_elapsedTime);
			m_ppObjects[i]->UpdateTransform(NULL, false);
			m_ppObjects[i]->Render(pd3dCommandList, pCamera, nPipelineState);
		}
	}
#endif
}

void CSkinnedAnimationObjectShader::MappingUserToEvilbear(char id, char matID)
{
	if (id < MAX_USER && matID <max_Material)
	{
		bool isExist = false;
		for(auto& enemyID :m_mapMaterial)
		{
			if(enemyID.first == id)
			{
				enemyID.second = matID;
				isExist = true;
			}
		}
		if (isExist == false) 
		{
			m_mapMaterial.emplace(id, matID);
		}	
	}
}

void CSkinnedAnimationObjectShader::MaterialClear()
{
	for (auto iter = m_mapMaterial.begin(); iter != m_mapMaterial.end(); )
		iter = m_mapMaterial.erase(iter);		
	
	m_mapMaterial.clear();
}
#include "../../../../Stdafx/Stdafx.h"
#include "SkinnedAnimationObjectShader.h"
#include "../../../../ResourceManager/ResourceManager.h"
#include "../../../../GameObject/GameObject.h"
#include "../../../../GameObject/Terrain/Terrain.h"
#include "../../../../GameObject/EvilBear/EvilBear.h"
#include "../../../../GameObject/Shadow/Shadow.h"
#include "../../../../FrameTransform/FrameTransform.h"
#include "../../../../../../../Server/FreezeBombServer/FreezeBombServer/protocol.h"

CSkinnedAnimationObjectShader::CSkinnedAnimationObjectShader()
	:m_userID{ 0 }
	
{
}

CSkinnedAnimationObjectShader::~CSkinnedAnimationObjectShader()
{
	m_vMaterial.clear();
	//m_EvilbearMap.clear();
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
		dynamic_cast<CEvilBear*>(m_ppObjects[0])->SetPlayerName(L"이우상");


		m_ppObjects[1] = new CEvilBear(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, CGameObject::MATERIALTYPE::BROWN);
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
		dynamic_cast<CEvilBear*>(m_ppObjects[1])->SetPlayerName(L"염혜린");



		m_ppObjects[2] = new CEvilBear(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, CGameObject::MATERIALTYPE::WHITE);
		m_ppObjects[2]->SetChild((*Model).second->m_pModelRootObject, true);
		m_ppObjects[2]->m_pAnimationController = new CAnimationController(1, (*Model).second->m_pAnimationSets);
		m_ppObjects[2]->m_pAnimationController->SetTrackAnimationSet(0, m_ppObjects[2]->m_pAnimationController->ATTACK);
		// 애니메이션의 시작위치를 다르게 준다. 그러면 같은 동작이더라도 다르게 애니메이션함
		//m_ppObjects[2]->m_pAnimationController->SetTrackPosition(0, 0.95f);

		m_ppObjects[2]->m_pSkinningBoneTransforms = new CSkinningBoneTransforms(pd3dDevice, pd3dCommandList, (*Model).second);
		dynamic_cast<CEvilBear*>(m_ppObjects[2])->SetPlayerName(L"하양이");



		m_ppObjects[3] = new CEvilBear(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, CGameObject::MATERIALTYPE::BLACK);
		m_ppObjects[3]->SetChild((*Model).second->m_pModelRootObject, true);
		m_ppObjects[3]->m_pAnimationController = new CAnimationController(1, (*Model).second->m_pAnimationSets);
		m_ppObjects[3]->m_pAnimationController->SetTrackAnimationSet(0, m_ppObjects[3]->m_pAnimationController->RAISEHAND);

		m_ppObjects[3]->m_pSkinningBoneTransforms = new CSkinningBoneTransforms(pd3dDevice, pd3dCommandList, (*Model).second);
		dynamic_cast<CEvilBear*>(m_ppObjects[3])->SetPlayerName(L"까망이");



		m_ppObjects[4] = new CEvilBear(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, CGameObject::MATERIALTYPE::BLUE);
		m_ppObjects[4]->SetChild((*Model).second->m_pModelRootObject, true);
		m_ppObjects[4]->m_pAnimationController = new CAnimationController(1, (*Model).second->m_pAnimationSets);
		m_ppObjects[4]->m_pAnimationController->SetTrackAnimationSet(0, m_ppObjects[4]->m_pAnimationController->RUNBACKWARD);
		m_ppObjects[4]->m_pSkinningBoneTransforms = new CSkinningBoneTransforms(pd3dDevice, pd3dCommandList, (*Model).second);
		dynamic_cast<CEvilBear*>(m_ppObjects[4])->SetPlayerName(L"파랑이");

		m_ppObjects[5] = new CEvilBear(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, CGameObject::MATERIALTYPE::PANDA);
		m_ppObjects[5]->SetChild((*Model).second->m_pModelRootObject, true);
		m_ppObjects[5]->m_pAnimationController = new CAnimationController(1, (*Model).second->m_pAnimationSets);
		m_ppObjects[5]->m_pAnimationController->SetTrackAnimationSet(0, m_ppObjects[4]->m_pAnimationController->RUNBACKWARD);
		m_ppObjects[5]->m_pSkinningBoneTransforms = new CSkinningBoneTransforms(pd3dDevice, pd3dCommandList, (*Model).second);
		dynamic_cast<CEvilBear*>(m_ppObjects[5])->SetPlayerName(L"판다");


		
		for (int i = 0; i < m_nObjects; ++i)
		{
			// 애니메이션 Transform을 각자 갖고있게 하기위해
			m_ppObjects[i]->m_pSkinningBoneTransforms = new CSkinningBoneTransforms(pd3dDevice, pd3dCommandList, (*Model).second);
			// 메쉬의 월드행렬을 각자 갖고있게 하기위해
			m_ppObjects[i]->m_pFrameTransform = new CFrameTransform(pd3dDevice, pd3dCommandList, (*Model).second);
		}

	}

	CAnimationCallbackHandler* pRunAnimationCallback = new CSoundCallbackHandler();
	CAnimationCallbackHandler* pRaiseHandAnimationCallback = new CSoundCallbackHandler();
	CAnimationCallbackHandler* pDieAnimationCallback = new CSoundCallbackHandler();
	CAnimationCallbackHandler* pAttackAnimationCallback = new CSoundCallbackHandler();
	CAnimationCallbackHandler* pBackRunAnimationCallback = new CSoundCallbackHandler();
	
	for(int i=0; i < m_nObjects; i++)		//플레이어 수만큼 사운드 효과 설정해준다
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


			m_ppObjects[i]->m_pAnimationController->SetCallbackKeys(m_ppObjects[i]->m_pAnimationController->RUNBACKWARD, 2);
			m_ppObjects[i]->m_pAnimationController->SetCallbackKey(m_ppObjects[i]->m_pAnimationController->RUNBACKWARD, 0, 0.1f, (void*)CEvilBear::MUSIC_ENUM::FOOTSTEP);
			m_ppObjects[i]->m_pAnimationController->SetCallbackKey(m_ppObjects[i]->m_pAnimationController->RUNBACKWARD, 1, 0.3f, (void*)CEvilBear::MUSIC_ENUM::FOOTSTEP);
			m_ppObjects[i]->m_pAnimationController->SetAnimationCallbackHandler(m_ppObjects[i]->m_pAnimationController->RUNBACKWARD, pBackRunAnimationCallback,
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

		iter = map.find(CEvilBear::MUSIC_ENUM::DIE);

		if(iter != map.end())
		{
			m_ppObjects[i]->m_pAnimationController->SetCallbackKeys(m_ppObjects[i]->m_pAnimationController->DIE,1);
			m_ppObjects[i]->m_pAnimationController->SetCallbackKey(m_ppObjects[i]->m_pAnimationController->DIE, 0, 0.1f,(void*)CEvilBear::MUSIC_ENUM::DIE/*MAKEINTRESOURCE(IDR_WAVE2)*/);
			m_ppObjects[i]->m_pAnimationController->SetAnimationCallbackHandler(m_ppObjects[i]->m_pAnimationController->DIE, pDieAnimationCallback,
			dynamic_cast<CEvilBear*>(m_ppObjects[i])->GetSoundData());
		}
		iter = map.find(CEvilBear::MUSIC_ENUM::ATTACK);

		if(iter != map.end())
		{
			m_ppObjects[i]->m_pAnimationController->SetCallbackKeys(m_ppObjects[i]->m_pAnimationController->ATTACK, 1);
			m_ppObjects[i]->m_pAnimationController->SetCallbackKey(m_ppObjects[i]->m_pAnimationController->ATTACK, 0, 0.2f, (void*)CEvilBear::MUSIC_ENUM::ATTACK);
			m_ppObjects[i]->m_pAnimationController->SetAnimationCallbackHandler(m_ppObjects[i]->m_pAnimationController->ATTACK, pAttackAnimationCallback,
			 dynamic_cast<CEvilBear*>(m_ppObjects[i])->GetSoundData());
		}

	}

	XMFLOAT3 Position;
	for (int i = 0; i < m_nObjects; ++i)
	{
		Position.x = Random(0.f, 500.f);
		Position.z = Random(0.f, 300.f);
		//Position.x = i * 10;
		//Position.z = 0;
		//Position.y = pTerrain->GetHeight(Position.x, Position.z);
		Position.y = 0.f;
		m_ppObjects[i]->SetPosition(Position);
		m_ppObjects[i]->SetScale(10, 10, 10);

		m_ppObjects[i]->setID("EvilBear");
		auto iter = Context.find(m_ppObjects[i]->getID());
		if (iter != Context.end())
			m_ppObjects[i]->SetOOBB((*iter).second->m_xmf3Center, (*iter).second->m_xmf3Extent, XMFLOAT4(0, 0, 0, 1));

		dynamic_cast<CEvilBear*>(m_ppObjects[i])->Initialize_Shadow((*Model).second, m_ppObjects[i]);
	}


	//임시 테스트
	//MappingUserToEvilbear(3, 3);//까망 (이름은 까망이)
	//MappingUserToEvilbear(0, 0);//핑크 (이름은 이우상)
	//MappingUserToEvilbear(1, 2);//하양(이름은 염혜린)

}

void CSkinnedAnimationObjectShader::AnimateObjects(float fTimeElapsed, CCamera* pCamera,CPlayer* pPlayer)
{
	m_elapsedTime = fTimeElapsed;
}

void CSkinnedAnimationObjectShader::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState)
{

#ifdef _WITH_SERVER_
	for (int i = 0; i < m_vMaterial.size(); ++i)
	{
		char matID = m_vMaterial[i].second;
		char id = m_vMaterial[i].first;
		if (dynamic_cast<CEvilBear*>(m_ppObjects[id])->GetActive())
		{
			m_ppObjects[id]->Animate(m_elapsedTime);
			m_ppObjects[id]->UpdateTransform(NULL);
			m_ppObjects[id]->Render(pd3dCommandList, m_ppObjects[id]->GetIsHammer(), m_ppObjects[id]->GetIsBomb(), m_ppObjects[id]->GetBoolIce(), matID, pCamera, nPipelineState);
		}
	}
#else
	for (int i = 0; i < m_nObjects; ++i)
	{
		if (m_ppObjects[i])
		{
			m_ppObjects[i]->Animate(m_elapsedTime);
			m_ppObjects[i]->UpdateTransform(NULL);
			m_ppObjects[i]->Render(pd3dCommandList, m_ppObjects[i]->GetIsHammer(), m_ppObjects[i]->GetIsBomb(), m_ppObjects[i]->GetBoolIce(), m_ppObjects[i]->GetMaterialID(), pCamera, nPipelineState);
		}
	}
#endif
}

//
void CSkinnedAnimationObjectShader::MappingUserToEvilbear(char id,char matID)
{
	if (id < MAX_USER && matID <max_Material)
	{
		dynamic_cast<CEvilBear*>(m_ppObjects[id])->SetActive(true);
		m_vMaterial.emplace_back(id,matID);
	}
}
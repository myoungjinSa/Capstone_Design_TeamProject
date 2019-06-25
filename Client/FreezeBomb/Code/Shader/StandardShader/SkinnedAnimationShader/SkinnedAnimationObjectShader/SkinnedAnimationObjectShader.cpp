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


CSkinnedAnimationObjectShader::CSkinnedAnimationObjectShader()	:m_userID{ 0 }	
{
}

CSkinnedAnimationObjectShader::~CSkinnedAnimationObjectShader()
{
	m_vMaterial.clear();
	m_mapMusicList.clear();
	ReleaseSound();
}

void CSkinnedAnimationObjectShader::InitializeSound()
{
	m_pSound = new CSoundSystem;

	m_SoundCount = 4;
		
	m_SoundList = new const char*[m_SoundCount];

	m_SoundList[0] = "../Resource/Sound/BtnDown03.wav";
	m_SoundList[1] = "../Resource/Sound/bell1.wav";
	m_SoundList[2] = "../Resource/Sound/Bomb.mp3";
	m_SoundList[3] = "../Resource/Sound/Effect/HammerSwing.wav";
	

	std::string s0(m_SoundList[0]);
	std::string s1(m_SoundList[1]);
	std::string s2(m_SoundList[2]);
	std::string s3(m_SoundList[3]);
	
	////m_SoundList[1] = "../Resource/Sound/bell1.wav";


	m_mapMusicList.emplace(FOOTSTEP, s0);
	m_mapMusicList.emplace(USETIMER, s1);
	m_mapMusicList.emplace(DIE, s2);
	m_mapMusicList.emplace(ATTACK, s3);
	

	if(m_pSound)
	{
		m_pSound->Initialize(m_SoundCount, m_SoundList, FMOD_LOOP_OFF);
	}
}
void CSkinnedAnimationObjectShader::ReleaseSound()
{
	
	if (m_SoundList)
		delete[] m_SoundList;
	if (m_pSound)
		m_pSound->Release();
}

void CSkinnedAnimationObjectShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature,
	const map<string, CLoadedModelInfo*>& ModelMap, const map<string, Bounds*>& Context,const int& nPlayerCount, void* pContext)
{
	CTerrain* pTerrain = (CTerrain *)pContext;
	m_nObjects = nPlayerCount;

	m_ppObjects = new CGameObject*[m_nObjects];
	
	InitializeSound();
	auto Model = ModelMap.find("EvilBear");
	if (Model != ModelMap.end())
	{
		m_ppObjects[0] = new CEvilBear(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, CGameObject::MATERIALTYPE::PINK);
		m_ppObjects[0]->SetChild((*Model).second->m_pModelRootObject, true);
		m_ppObjects[0]->m_pAnimationController = new CAnimationController(1, (*Model).second->m_pAnimationSets);
		// 0번 트랙에 0번 애니메이션을 Set
		m_ppObjects[0]->m_pAnimationController->SetTrackAnimationSet(0, m_ppObjects[0]->m_pAnimationController->IDLE);
		//m_ppObjects[0]->m_pAnimationController->SetTrackSpeed(0, 1.0f / m_nObjects);
		//m_ppObjects[0]->m_pAnimationController->SetAnimationState(m_ppObjects[0]->m_pAnimationController->IDLE);
		// 1번 트랙에 1번 애니메이션을 Set
		//m_ppObjects[0]->m_pAnimationController->SetTrackAnimationSet(1, 1);
		// 0번 트랙에 가중치를 80%
		//m_ppObjects[0]->m_pAnimationController->SetTrackWeight(0, 0.8f);
		// 1번 트랙에 가중치를 20% 
		//m_ppObjects[0]->m_pAnimationController->SetTrackWeight(1, 0.2f);

		m_ppObjects[0]->m_pSkinningBoneTransforms = new CSkinningBoneTransforms(pd3dDevice, pd3dCommandList, (*Model).second);
		dynamic_cast<CEvilBear*>(m_ppObjects[0])->SetPlayerName(L"핑크");


		m_ppObjects[1] = new CEvilBear(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, CGameObject::MATERIALTYPE::BROWN);
		m_ppObjects[1]->SetChild((*Model).second->m_pModelRootObject, true);
		m_ppObjects[1]->m_pAnimationController = new CAnimationController(1, (*Model).second->m_pAnimationSets);
		//m_ppObjects[1]->m_pAnimationController->SetTrackAnimationSet(1, 2);
		m_ppObjects[1]->m_pAnimationController->SetTrackAnimationSet(0, m_ppObjects[1]->m_pAnimationController->RUNFAST);
		//m_ppObjects[1]->m_pAnimationController->SetTrackSpeed(0, 1.0f / m_nObjects);
		//m_ppObjects[1]->m_pAnimationController->SetAnimationState(m_ppObjects[1]->m_pAnimationController->RAISEHAND);
		//m_ppObjects[1]->m_pAnimationController->SetTrackSpeed(0, 0.8f);
		//m_ppObjects[1]->m_pAnimationController->SetTrackWeight(0, 0.9);
		//m_ppObjects[1]->m_pAnimationController->SetTrackWeight(1, 0.1);
		// 애니메이션의 속도를 0.25로 주어서 느리게 애니메이션 동작을 하도록 Set
		//m_ppObjects[1]->m_pAnimationController->SetTrackSpeed(0, 0.25f);
		m_ppObjects[1]->m_pSkinningBoneTransforms = new CSkinningBoneTransforms(pd3dDevice, pd3dCommandList, (*Model).second);
		dynamic_cast<CEvilBear*>(m_ppObjects[1])->SetPlayerName(L"브라운");

		m_ppObjects[2] = new CEvilBear(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, CGameObject::MATERIALTYPE::WHITE);
		m_ppObjects[2]->SetChild((*Model).second->m_pModelRootObject, true);
		m_ppObjects[2]->m_pAnimationController = new CAnimationController(1, (*Model).second->m_pAnimationSets);
		m_ppObjects[2]->m_pAnimationController->SetTrackAnimationSet(0, m_ppObjects[2]->m_pAnimationController->ATTACK);
		
		//m_ppObjects[2]->m_pAnimationController->SetAnimationState(m_ppObjects[2]->m_pAnimationController->ATTACK);
		// 애니메이션의 시작위치를 다르게 준다. 그러면 같은 동작이더라도 다르게 애니메이션함
		//m_ppObjects[2]->m_pAnimationController->SetTrackPosition(0, 0.95f);
		//m_ppObjects[2]->m_pAnimationController->SetTrackSpeed(0, 1.0f / m_nObjects);
		m_ppObjects[2]->m_pSkinningBoneTransforms = new CSkinningBoneTransforms(pd3dDevice, pd3dCommandList, (*Model).second);
		dynamic_cast<CEvilBear*>(m_ppObjects[2])->SetPlayerName(L"화이트");

		m_ppObjects[3] = new CEvilBear(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, CGameObject::MATERIALTYPE::BLACK);
		m_ppObjects[3]->SetChild((*Model).second->m_pModelRootObject, true);
		m_ppObjects[3]->m_pAnimationController = new CAnimationController(1, (*Model).second->m_pAnimationSets);
		m_ppObjects[3]->m_pAnimationController->SetTrackAnimationSet(0, m_ppObjects[3]->m_pAnimationController->RAISEHAND);
		//m_ppObjects[3]->m_pAnimationController->SetAnimationState(m_ppObjects[3]->m_pAnimationController->RAISEHAND);
		m_ppObjects[3]->m_pSkinningBoneTransforms = new CSkinningBoneTransforms(pd3dDevice, pd3dCommandList, (*Model).second);
		//m_ppObjects[3]->m_pAnimationController->SetTrackSpeed(0, 1.0f / m_nObjects);
		dynamic_cast<CEvilBear*>(m_ppObjects[3])->SetPlayerName(L"블랙");

		m_ppObjects[4] = new CEvilBear(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, CGameObject::MATERIALTYPE::BLUE);
		m_ppObjects[4]->SetChild((*Model).second->m_pModelRootObject, true);
		m_ppObjects[4]->m_pAnimationController = new CAnimationController(1, (*Model).second->m_pAnimationSets);
		m_ppObjects[4]->m_pAnimationController->SetTrackAnimationSet(0, m_ppObjects[4]->m_pAnimationController->RUNBACKWARD);
		//m_ppObjects[4]->m_pAnimationController->SetAnimationState(m_ppObjects[4]->m_pAnimationController->RUNBACKWARD);
		//m_ppObjects[4]->m_pAnimationController->SetTrackSpeed(0, 1.0f / m_nObjects);
		m_ppObjects[4]->m_pSkinningBoneTransforms = new CSkinningBoneTransforms(pd3dDevice, pd3dCommandList, (*Model).second);
		dynamic_cast<CEvilBear*>(m_ppObjects[4])->SetPlayerName(L"블루");


		m_ppObjects[5] = new CEvilBear(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, CGameObject::MATERIALTYPE::PANDA);
		m_ppObjects[5]->SetChild((*Model).second->m_pModelRootObject, true);
		m_ppObjects[5]->m_pAnimationController = new CAnimationController(1, (*Model).second->m_pAnimationSets);
		m_ppObjects[5]->m_pAnimationController->SetTrackAnimationSet(0, m_ppObjects[5]->m_pAnimationController->IDLE);
		//m_ppObjects[5]->m_pAnimationController->SetAnimationState(m_ppObjects[5]->m_pAnimationController->IDLE);
		//m_ppObjects[5]->m_pAnimationController->SetTrackSpeed(0, 1.0f / m_nObjects);
		m_ppObjects[5]->m_pSkinningBoneTransforms = new CSkinningBoneTransforms(pd3dDevice, pd3dCommandList, (*Model).second);
		dynamic_cast<CEvilBear*>(m_ppObjects[5])->SetPlayerName(L"펜더");

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
		auto map = m_mapMusicList;
		
		auto iter = map.find(FOOTSTEP);
		
		if(iter != map.end())
		{
			m_ppObjects[i]->m_pAnimationController->SetCallbackKeys(m_ppObjects[i]->m_pAnimationController->RUNFAST, 2);
			m_ppObjects[i]->m_pAnimationController->SetCallbackKey(m_ppObjects[i]->m_pAnimationController->RUNFAST, 0, 0.3f,(void*)FOOTSTEP/*MAKEINTRESOURCE(IDR_WAVE2)*/);
			m_ppObjects[i]->m_pAnimationController->SetCallbackKey(m_ppObjects[i]->m_pAnimationController->RUNFAST, 1, 0.6f, (void*)FOOTSTEP/*MAKEINTRESOURCE(IDR_WAVE2)*/);
			m_ppObjects[i]->m_pAnimationController->SetAnimationCallbackHandler(m_ppObjects[i]->m_pAnimationController->RUNFAST, pRunAnimationCallback,
				GetSoundData());


			m_ppObjects[i]->m_pAnimationController->SetCallbackKeys(m_ppObjects[i]->m_pAnimationController->RUNBACKWARD, 2);
			m_ppObjects[i]->m_pAnimationController->SetCallbackKey(m_ppObjects[i]->m_pAnimationController->RUNBACKWARD, 0, 0.1f, (void*)FOOTSTEP);
			m_ppObjects[i]->m_pAnimationController->SetCallbackKey(m_ppObjects[i]->m_pAnimationController->RUNBACKWARD, 1, 0.3f, (void*)FOOTSTEP);
			m_ppObjects[i]->m_pAnimationController->SetAnimationCallbackHandler(m_ppObjects[i]->m_pAnimationController->RUNBACKWARD, pBackRunAnimationCallback,
				GetSoundData());

		}

		iter = map.find(USETIMER);
		
		if(iter != map.end())
		{
			m_ppObjects[i]->m_pAnimationController->SetCallbackKeys(m_ppObjects[i]->m_pAnimationController->RAISEHAND,1);
			m_ppObjects[i]->m_pAnimationController->SetCallbackKey(m_ppObjects[i]->m_pAnimationController->RAISEHAND, 0, 0.3f,(void*)USETIMER/*MAKEINTRESOURCE(IDR_WAVE2)*/);
			m_ppObjects[i]->m_pAnimationController->SetAnimationCallbackHandler(m_ppObjects[i]->m_pAnimationController->RAISEHAND, pRaiseHandAnimationCallback,
			GetSoundData());
		}

		iter = map.find(DIE);

		if(iter != map.end())
		{
			m_ppObjects[i]->m_pAnimationController->SetCallbackKeys(m_ppObjects[i]->m_pAnimationController->DIE,1);
			m_ppObjects[i]->m_pAnimationController->SetCallbackKey(m_ppObjects[i]->m_pAnimationController->DIE, 0, 0.1f,(void*)DIE/*MAKEINTRESOURCE(IDR_WAVE2)*/);
			m_ppObjects[i]->m_pAnimationController->SetAnimationCallbackHandler(m_ppObjects[i]->m_pAnimationController->DIE, pDieAnimationCallback,
			GetSoundData());
		}
		iter = map.find(ATTACK);

		if(iter != map.end())
		{
			m_ppObjects[i]->m_pAnimationController->SetCallbackKeys(m_ppObjects[i]->m_pAnimationController->ATTACK, 1);
			m_ppObjects[i]->m_pAnimationController->SetCallbackKey(m_ppObjects[i]->m_pAnimationController->ATTACK, 0, 0.2f, (void*)ATTACK);
			m_ppObjects[i]->m_pAnimationController->SetAnimationCallbackHandler(m_ppObjects[i]->m_pAnimationController->ATTACK, pAttackAnimationCallback,
			 GetSoundData());
		}
	}

	// 임시로 플레이어 위치 고정
	int index = 0;
	m_ppObjects[index]->SetPosition(100, 0, 100);
	m_ppObjects[index++]->SetIsICE(true);

	m_ppObjects[index++]->SetPosition(58, 0, 180);
	m_ppObjects[index++]->SetPosition(192, 0, 230);
	m_ppObjects[index++]->SetPosition(386, 0, 206);
	m_ppObjects[index++]->SetPosition(250, 0, 50);
	
	m_ppObjects[index]->SetPosition(70, 0, 50);
	m_ppObjects[index++]->SetIsICE(true);


	for (int i = 0; i < m_nObjects; ++i)
	{
		m_ppObjects[i]->SetScale(10, 10, 10);

		m_ppObjects[i]->setID("EvilBear");
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

#ifdef _WITH_SERVER_
	for (int i = 0; i < m_vMaterial.size(); ++i)
	{
		char matID = m_vMaterial[i].second;
		char id = m_vMaterial[i].first;
		
		m_ppObjects[id]->Animate(m_elapsedTime);
		m_ppObjects[id]->UpdateTransform(NULL);
		m_ppObjects[id]->Render(pd3dCommandList, m_ppObjects[id]->GetIsHammer(), m_ppObjects[id]->GetIsBomb(), m_ppObjects[id]->GetBoolIce(), matID, pCamera, nPipelineState);
		
	}
	//cout << "m_vMaterial Size" << m_vMaterial.size() << "\n";
#else
	for (int i = 0; i < m_nObjects; ++i)
	{
		if (m_ppObjects[i])
		{
			m_ppObjects[i]->Animate(m_elapsedTime);
			m_ppObjects[i]->UpdateTransform(NULL);
			m_ppObjects[i]->Render(pd3dCommandList, pCamera, nPipelineState);
		}
	}
#endif
}

void CSkinnedAnimationObjectShader::MappingUserToEvilbear(char id,char matID)
{
	if (id < MAX_USER && matID <max_Material)
	{
		m_vMaterial.emplace_back(id,matID);
	}
}
#include "../../Stdafx/Stdafx.h"
#include <vector>
#include "../../SoundSystem/SoundSystem.h"
#include "EvilBear.h"
#include "../Shadow/Shadow.h"
#include "../../Material/Material.h"
#include "../../Shader/Shader.h"
#include "../../FrameTransform/FrameTransform.h"

CEvilBear::CEvilBear(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature,int matID)
	:m_pSound{nullptr}, m_SoundCount{0}
{
	m_matID = matID;
//	InitializeSound();
}

CEvilBear::~CEvilBear()
{
	//m_pSound->Release();

	if (m_pShadow)
		delete m_pShadow;
}


void CEvilBear::InitializeSound()
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
		m_pSound->Initialize(m_SoundCount, m_SoundList,FMOD_LOOP_OFF);
	}
}

void CEvilBear::Initialize_Shadow(CLoadedModelInfo* pLoadedModel, CGameObject* pGameObject)
{
	m_pShadow = new CShadow(pLoadedModel, this);
}

void CEvilBear::Render(ID3D12GraphicsCommandList *pd3dCommandList, bool bHammer, bool bBomb, bool bIce, int matID, CCamera* pCamera, int nPipelineState)
{
	if (IsVisible(pCamera) == true)
	{
		CGameObject::Render(pd3dCommandList, bHammer, bBomb, bIce, matID, pCamera, GameObject);

		if (m_pShadow)
			m_pShadow->Render(pd3dCommandList, bHammer, bBomb, bIce, matID, pCamera, GameObject_Shadow);
	}
}

void CEvilBear::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera* pCamera, int nPipelineState)
{
}



void CEvilBear::Animate(float fTimeElapsed)
{
	// 루트 오브젝트를 제외한 나머지는 모두 nullptr이다. 왜냐하면, 루트 오브젝트를 제어하기 위함이므로
	if (m_pAnimationController) 
		m_pAnimationController->AdvanceTime(fTimeElapsed,(float*)&m_fDistanceToTarget);

	m_xmOOBBTransformed.Transform(m_xmOOBB, XMLoadFloat4x4(&m_xmf4x4World));
	XMStoreFloat4(&m_xmOOBBTransformed.Orientation, XMQuaternionNormalize(XMLoadFloat4(&m_xmOOBBTransformed.Orientation)));

	if (m_pSibling) 
		m_pSibling->Animate(fTimeElapsed);
	if (m_pChild) 
		m_pChild->Animate(fTimeElapsed);
}
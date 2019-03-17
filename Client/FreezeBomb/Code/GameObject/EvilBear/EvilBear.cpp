#include "../../Stdafx/Stdafx.h"
#include <vector>
#include "../../SoundSystem/SoundSystem.h"
#include "EvilBear.h"
#include "../Shadow/Shadow.h"
#include "../../Material/Material.h"
#include "../../Shader/Shader.h"

CEvilBear::CEvilBear(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature,int matID)
	:m_pSound{nullptr},
	m_pShadow{nullptr},
	m_SoundCount{0}
{
	m_matID = matID;
	InitializeSound();

}

CEvilBear::~CEvilBear()
{
	m_pSound->Release();
}


void CEvilBear::InitializeSound()
{
	m_pSound = new CSoundSystem;


	m_SoundCount = 2;
	
	
	m_SoundList = new const char*[m_SoundCount];

	m_SoundList[0] = "../Resource/Sound/BtnDown03.wav";
	m_SoundList[1] = "../Resource/Sound/bell1.wav";
	
	std::string s0(m_SoundList[0]);
	std::string s1(m_SoundList[1]);
	////m_SoundList[1] = "../Resource/Sound/bell1.wav";


	m_mapMusicList.emplace(FOOTSTEP, s0);
	m_mapMusicList.emplace(USETIMER, s1);


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
	CGameObject::Render(pd3dCommandList, bHammer, bBomb, bIce, matID, pCamera, GameObject);

	if (m_pShadow)
		m_pShadow->Render(pd3dCommandList, bHammer, bBomb, bIce, matID, pCamera, GameObject_Shadow);
}


void CEvilBear::ReleaseSound()
{
	if(m_pSound)
	{
		m_pSound->Release();
	}
	
}

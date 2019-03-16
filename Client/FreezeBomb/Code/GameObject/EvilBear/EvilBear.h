#pragma once

#include "../GameObject.h"

class CShadow;
class CSoundSystem;

class CEvilBear : public CGameObject
{
public:
	CEvilBear(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature,int matID);
	virtual ~CEvilBear();

	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, bool bHammer, bool bBomb, bool bIce, int matID, CCamera *pCamera, int nPipelineState = GameObject);


	//void SetSoundPtr();
	enum MUSIC_ENUM
	{
		FOOTSTEP=1,
		ATTACK
	};

	
	void* GetSoundData() const { return static_cast<void*>(m_pSound); }
	void InitializeSound();
	void ReleaseSound();
	void Initialize_Shadow(CLoadedModelInfo* pLoadedModel, CGameObject* pGameObject);	
public:

	std::map<MUSIC_ENUM, std::string> m_mapMusicList;
private:
	
	const char**	m_SoundList;
	int			m_SoundCount;
	CSoundSystem* m_pSound;

	CShadow*	m_pShadow;

};


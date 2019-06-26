#pragma once

#include "../SkinnedAnimationShader.h"

class CSoundSystem;
class CLoadedModelInfo;
struct Bounds;
class CSkinnedAnimationObjectShader : public CSkinnedAnimationShader
{
public:
	CSkinnedAnimationObjectShader();
	virtual ~CSkinnedAnimationObjectShader();

	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature,
		const map<string, CLoadedModelInfo*>& ModelMap, const map<string, Bounds*>& Context,const int& nPlayerCount, void *pContext = nullptr);
	virtual void AnimateObjects(float fTimeElapsed, CCamera *pCamera,CPlayer* pPlayer = nullptr);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState);

	void MappingUserToEvilbear(char id,char matID);
	void InitializeSound();
	void ReleaseSound();

	void* GetSoundData() const { return static_cast<void*>(m_pSound); }
public:

	enum MUSIC_ENUM
	{
		FOOTSTEP=1,
		USETIMER,
		DIE,
		ATTACK,
		ELECTRIC
	};
	std::map<MUSIC_ENUM, std::string> m_mapMusicList;
	vector<pair<char,char>> m_vMaterial;
private:
	
	const char**	m_SoundList;
	int			m_SoundCount;
	CSoundSystem* m_pSound;

	const int max_Material = 6;
	//map<int, CGameObject*> m_EvilbearMap;
	char  m_userID;
	int   m_userCount;
	float m_elapsedTime = 0.f;
};
#pragma once

#include "FMod/Include/fmod.hpp"
#include "FMod/Include/fmod_errors.h"
#pragma comment(lib,"../Resource/FMod/lib/fmodL64_vc.lib")
#pragma comment(lib,"../Resource/FMod/lib/fmodstudioL64_vc.lib")

using namespace FMOD;


class CSoundSystem
{
public:
	CSoundSystem();
	~CSoundSystem();

public:
	void Initialize(const int soundNum,const char** musicList,int nFlags);
	void Play(const int SoundNum,float volume =1.0f);
	void PlayIndex(unsigned int index,float volume=1.0f);
	void StopIndex(unsigned int index);
	void Release();

	const int& GetSoundCount()const { return m_soundCount; }

private:
	System* pSystem;
	Sound**	pSound;
	int m_soundCount;
	Channel* pChannel[5];		//»ç¿îµå ¹Í½Ì¿¡ ÇÊ¿ä
};
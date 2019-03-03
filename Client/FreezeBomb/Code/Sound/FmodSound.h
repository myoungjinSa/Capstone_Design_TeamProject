#pragma once

#include "FMOD\Include\fmod.hpp"
#include "FMOD\Include\fmod_errors.h"

#pragma comment(lib,"fmodL64_vc.lib")
#pragma comment(lib,"fmodstudioL64_vc.lib")

using namespace FMOD;

class CSoundSystem
{
public:
	CSoundSystem();
	~CSoundSystem();


public:
	void Initialize(const int soundNum,const char** musicList);
	void Play(int index);
	void Release();

private:
	System* pSystem;
	Sound**	pSound;
	int m_soundCount;
	Channel* pChannel[1];		//»ç¿îµå ¹Í½Ì¿¡ ÇÊ¿ä
};
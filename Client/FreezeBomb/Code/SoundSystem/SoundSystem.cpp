#include "SoundSystem.h"

CSoundSystem::CSoundSystem() : m_soundCount(0), pSystem(nullptr), pSound(nullptr)
{
	pChannel[0] = nullptr;
}

CSoundSystem::~CSoundSystem()
{
}

void CSoundSystem::Initialize(const int soundNum ,const char** musicList)
{
	m_soundCount = soundNum;
	pSound = new Sound*[m_soundCount];


	System_Create(&pSystem);

	const int maxSound = 4;

	if (soundNum <= maxSound) 
	{

		pSystem->init(soundNum, FMOD_INIT_NORMAL, nullptr);

		for (int i = 0; i < soundNum; i++) 
		{
			pSystem->createSound(musicList[i] , FMOD_LOOP_NORMAL | FMOD_DEFAULT, nullptr, &pSound[i]);
		}
	}

	
}


void CSoundSystem::Play(int Sound_num)
{
	for (int i = 0; i < Sound_num; i++) {
		pSystem->playSound(pSound[i], nullptr, 0, pChannel);
	}
}

void CSoundSystem::Release()
{
	for (int i = 0; i < m_soundCount; i++)
	{
		if (pSound[i])
			pSound[i]->release();
	}
	delete[] pSound;
	pSound = nullptr;

	if (pSystem) {
		pSystem->release();
	}
}
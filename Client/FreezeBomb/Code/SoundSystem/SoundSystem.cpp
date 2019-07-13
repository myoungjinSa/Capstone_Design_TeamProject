#include "SoundSystem.h"

System* CSoundSystem::m_pSystem = nullptr;
unordered_map<int, GameSound> CSoundSystem::m_GameSoundList;
extern volatile size_t g_FileSize;
extern bool g_IsSoundOn;

CSoundSystem::CSoundSystem() : m_soundCount(0), pSound(nullptr)
{
}

CSoundSystem::~CSoundSystem()
{
}

void CSoundSystem::Initialize(const int soundNum ,const char** musicList, int nFlags)
{
	System_Create(&pSystem);

	m_soundCount = soundNum;
	pSound = new Sound*[m_soundCount];

	const int maxSound = 8;

	if (soundNum <= maxSound) 
	{
		pSystem->init(soundNum, FMOD_INIT_NORMAL, nullptr);

		for (int i = 0; i < soundNum; i++) 
		{
			if(i == 2 || i == 3 || i == 4)
				pSystem->createSound(musicList[i] , FMOD_DEFAULT, nullptr, &pSound[i]);
			else
				pSystem->createSound(musicList[i] , nFlags | FMOD_DEFAULT, nullptr, &pSound[i]);
		}
	}
}

void CSoundSystem::AllSoundPlay(float volume)
{
	if (pSystem)
	{
		for (int i = 0; i < m_soundCount; i++)
		{
			if (pSound[i])
			{
				pSystem->playSound(pSound[i], nullptr, false, &pChannel[i]);
				pChannel[i]->setVolume(volume);
			}
		}
	}
}

void CSoundSystem::AllSoundStop()
{
	if (pSystem)
	{
		for (int i = 0; i < m_soundCount; i++)
		{
			if (pChannel[i])
			{
				pChannel[i]->setPaused(true);
			}
		}
	}
}

void CSoundSystem::SoundPlay(int index, float volume)
{
	if (pSystem)
	{
		if (pChannel[index])
		{
			pChannel[index]->setPaused(false);
		}
		else
		{
			pSystem->playSound(pSound[index], nullptr, false, &pChannel[index]);
			(volume > 0) ? pChannel[index]->setVolume(volume) : pChannel[index]->setVolume(0);
		}
	}
}

void CSoundSystem::SoundPause(int index)
{
	if (pSystem)
	{
		if (pChannel[index])
		{
			pChannel[index]->setPaused(true);
		}
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

	if (pSystem) 
		pSystem->release();
}

bool CSoundSystem::Initialize()
{
	FMOD_RESULT result = System_Create(&m_pSystem);
	if (result != FMOD_OK)
		return false;

	result = m_pSystem->init(32, FMOD_INIT_NORMAL, nullptr);
	if (result != FMOD_OK)
		return false;

	for (auto iter = m_GameSoundList.begin(); iter != m_GameSoundList.end(); ++iter)
	{
		cout << (*iter).second.m_filePath << endl;
		m_pSystem->createSound((*iter).second.m_filePath.c_str(), (*iter).second.m_mode, nullptr, &(*iter).second.m_pSound);
		if (result != FMOD_OK)
			return false;

		g_FileSize += (*iter).second.m_fileSize;
	}

	return true;
}

bool CSoundSystem::PlayingSound(int key, float volume)
{
	if (g_IsSoundOn == false)
		return false;

	auto iter = m_GameSoundList.find(key);
	if (iter != m_GameSoundList.end())
	{
		FMOD_RESULT result;

		// 처음 재생할 때,
		if ((*iter).second.m_pChannel == nullptr)
		{
			result = m_pSystem->playSound((*iter).second.m_pSound, nullptr, false, &(*iter).second.m_pChannel);
			(*iter).second.m_pChannel->setVolume(1.f);

			if (result != FMOD_OK)
				return false;

			return true;
		}

		// 재생 중인지 확인함
		bool isPlaying = false;
		(*iter).second.m_pChannel->isPlaying(&isPlaying);

		// 만약 재생중이라면, isPlaying이 true가 된다.
		if (isPlaying == true)
		{
			m_pSystem->update();
			return true;
		}

		result = m_pSystem->playSound((*iter).second.m_pSound, nullptr, false, &(*iter).second.m_pChannel);
		(*iter).second.m_pChannel->setVolume(1.f);

		if (result != FMOD_OK)
			return false;

		return true;
	}
	return false;
}

bool CSoundSystem::StopSound(int key)
{
	auto iter = m_GameSoundList.find(key);
	if (iter != m_GameSoundList.end())
	{
		// 재생된 적이 없으면, 멈출 필요도 없음.
		if ((*iter).second.m_pChannel == nullptr)
			return true;

		FMOD_RESULT result = (*iter).second.m_pChannel->stop();

		if (result != FMOD_OK)
			return false;

		return true;
	}
	return false;
}

bool CSoundSystem::PauseSound(int key)
{
	auto iter = m_GameSoundList.find(key);
	if (iter != m_GameSoundList.end())
	{
		FMOD_RESULT result;

		switch ((*iter).second.m_mode)
		{
		case FMOD_LOOP_OFF:
			result = (*iter).second.m_pChannel->stop();
			break;

		case FMOD_LOOP_NORMAL:
			result = (*iter).second.m_pChannel->setPaused(true);
			break;

		default:
			return false;
		}

		if (result != FMOD_OK)
			return false;

		return true;
	}

	return false;
}

bool CSoundSystem::ResumeSound(int key)
{
	auto iter = m_GameSoundList.find(key);
	if (iter != m_GameSoundList.end())
	{
		if ((*iter).second.m_mode == FMOD_LOOP_OFF)
			return false;

		FMOD_RESULT result = (*iter).second.m_pChannel->setPaused(false);

		if (result != FMOD_OK)
			return false;

		return true;
	}

	return false;
}
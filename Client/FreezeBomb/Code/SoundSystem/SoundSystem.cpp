#include "SoundSystem.h"

System* CSoundSystem::m_pSystem = nullptr;
unordered_map<int, GameSound> CSoundSystem::m_GameSoundList;
extern volatile size_t g_FileSize;
extern bool g_IsSoundOn;

CSoundSystem::CSoundSystem()
{
}

CSoundSystem::~CSoundSystem()
{
}

bool CSoundSystem::Initialize()
{
	FMOD_RESULT result = System_Create(&m_pSystem);
	if (result != FMOD_OK)
		return false;

	result = m_pSystem->init(MAX_CHANNEL, FMOD_INIT_NORMAL, nullptr);
	if (result != FMOD_OK)
		return false;

	for (auto iter = m_GameSoundList.begin(); iter != m_GameSoundList.end(); ++iter)
	{
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
			result = m_pSystem->playSound((*iter).second.m_pSound, nullptr, false, &(*iter).second.m_pChannel[0]);
			(*iter).second.m_pChannel[0]->setVolume(volume);

			if (result != FMOD_OK)
				return false;

			return true;
		}

		// 재생 중인지 확인함
		bool isPlaying = false;
		(*iter).second.m_pChannel[0]->isPlaying(&isPlaying);

		// 만약 재생중이라면, isPlaying이 true가 된다.
		if (isPlaying == true)
		{
			m_pSystem->update();
			return true;
		}

		result = m_pSystem->playSound((*iter).second.m_pSound, nullptr, false, &(*iter).second.m_pChannel[0]);
		(*iter).second.m_pChannel[0]->setVolume(volume);

		if (result != FMOD_OK)
			return false;

		return true;
	}
	return false;
}

bool CSoundSystem::PlayingPlayerSound(int key, int channel, float volume)
{
	if (g_IsSoundOn == false)
		return false;

	auto iter = m_GameSoundList.find(key);
	if (iter != m_GameSoundList.end())
	{
		FMOD_RESULT result;

		// 처음 재생할 때,
		if ((*iter).second.m_pChannel[channel] == nullptr)
		{
			result = m_pSystem->playSound((*iter).second.m_pSound, nullptr, false, &(*iter).second.m_pChannel[channel]);
			(*iter).second.m_pChannel[channel]->setVolume(volume);

			if (result != FMOD_OK)
				return false;

			return true;
		}

		// 재생 중인지 확인함
		bool isPlaying = false;
		(*iter).second.m_pChannel[channel]->isPlaying(&isPlaying);

		// 만약 재생중이라면, isPlaying이 true가 된다.
		if (isPlaying == true)
		{
			m_pSystem->update();
			return true;
		}

		result = m_pSystem->playSound((*iter).second.m_pSound, nullptr, false, &(*iter).second.m_pChannel[channel]);
		(*iter).second.m_pChannel[channel]->setVolume(volume);

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

		FMOD_RESULT result = (*iter).second.m_pChannel[0]->stop();

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
			result = (*iter).second.m_pChannel[0]->stop();
			break;

		case FMOD_LOOP_NORMAL:
			result = (*iter).second.m_pChannel[0]->setPaused(true);
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

		FMOD_RESULT result = (*iter).second.m_pChannel[0]->setPaused(false);

		if (result != FMOD_OK)
			return false;

		return true;
	}

	return false;
}

void CSoundSystem::AllSoundVolume(float volume)
{
	for (auto iter = m_GameSoundList.begin(); iter != m_GameSoundList.end(); ++iter)
	{
		for (int i = 0; i < MAX_CHANNEL; ++i)
		{
			(*iter).second.m_pChannel[i]->setVolume(volume);
		}
	}
}

void CSoundSystem::Release()
{
	for (auto iter = m_GameSoundList.begin(); iter != m_GameSoundList.end(); )
	{
		(*iter).second.m_pSound->release();
		iter = m_GameSoundList.erase(iter);
	}

	m_pSystem->close();
	m_pSystem->release();
}
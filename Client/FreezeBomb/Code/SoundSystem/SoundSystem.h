#pragma once

#include "FMod/Include/fmod.hpp"
#include "FMod/Include/fmod_errors.h"
#pragma comment(lib,"../Resource/FMod/lib/fmodL64_vc.lib")
#pragma comment(lib,"../Resource/FMod/lib/fmodstudioL64_vc.lib")
using namespace FMOD;

#include <iostream>
#include <string>
#include <unordered_map>
using namespace std;

constexpr int MAX_CHANNEL = 7;

struct GameSound
{
	GameSound(string path, size_t size, int mode) 
		: m_filePath(path), m_fileSize(size), m_mode(mode) 
	{
		m_pSound = nullptr;
		for (int i = 0; i < 6; ++i)
			m_pChannel[i] = nullptr;
	}

	string				m_filePath;
	size_t				m_fileSize;
	FMOD_MODE	m_mode;	
	Sound*				m_pSound;
	// 플레이어의 인원 수 만큼 채널을 가져야 함
	Channel*			m_pChannel[MAX_CHANNEL];
};

class CSoundSystem
{
public:
	CSoundSystem();
	~CSoundSystem();

	static void AddGameSound(int type, string path, size_t size, int mode) { m_GameSoundList.emplace(type, GameSound(path, size, mode)); }
	static bool Initialize();
	static bool PlayingSound(int key, float volume = 1.0f);
	static bool PlayingPlayerSound(int key, int channel, float volume = 1.0f);
	static bool StopSound(int key);
	static bool PauseSound(int key);
	static bool ResumeSound(int key);
	static void AllSoundVolume(float volume);
	static void Release();

	enum SOUND_TYPE 
	{ 
		LOBBY_BGM, CHARACTER_SELECT, 
		INGAME_BGM, TIMER_WARNING, ICE_BREAK, GET_ITEM, CLICK, 
		RUN1, RUN2, BOMBEXPLOSION_EFFECT, HAMMERSWING_EFFECT, GOLDHAMMER_EFFECT, GOLDTIMER_EFFECT,
		FIRE_SOUND, CATCH
	};

private:
	static System* m_pSystem;
	static unordered_map<int, GameSound>	m_GameSoundList;
};

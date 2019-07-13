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

struct GameSound
{
	GameSound(string path, size_t size, int mode) 
		: m_filePath(path), m_fileSize(size), m_mode(mode), m_pSound(nullptr), m_pChannel(nullptr) { }

	string				m_filePath;
	size_t				m_fileSize;
	FMOD_MODE	m_mode;	
	Sound*				m_pSound;
	Channel*			m_pChannel;
};

class CSoundSystem
{
public:
	CSoundSystem();
	~CSoundSystem();

	void Initialize(const int soundNum, const char** musicList,int nFlags);

	void AllSoundPlay(float volume = 1.0f);
	void AllSoundStop();
	void SoundPlay(int index, float volume = 1.0f);

	void SoundPause(int index);

	void Release();

	const int& GetSoundCount()		const { return m_soundCount; }

	static void AddGameSound(int type, string path, size_t size, int mode) { m_GameSoundList.emplace(type, GameSound(path, size, mode)); }
	static bool Initialize();
	static bool PlayingSound(int key, float volume = 1.0f);
	static bool StopSound(int key);
	static bool PauseSound(int key);
	static bool ResumeSound(int key);
	static void UpdateSound() { m_pSystem->update(); };

	enum SOUND_TYPE { LOBBY_BGM, CHARACTER_SELECT, INGAME_BGM, TIMER_WARNING, ICE_BREAK, GET_ITEM, CLICK };

private:
	static System* m_pSystem;
	static unordered_map<int, GameSound>	m_GameSoundList;

	System* pSystem;
	Sound** pSound;
	int m_soundCount;
	Channel* pChannel[5];		//»ç¿îµå ¹Í½Ì¿¡ ÇÊ¿ä
};

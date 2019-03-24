#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>
#include <Windows.h>
#include <DirectXMath.h>
using namespace DirectX;

enum KEY
{
	KEY_IDLE		= 0x00,
	KEY_RIGHT	= 0x01,
	KEY_LEFT		= 0x02,
	KEY_UP			= 0x03,
	KEY_DOWN	= 0x04,
	KEY_CTRL		= 0x05,
	KEY_ALT		= 0x06,
	KEY_SHIFT	= 0x07,
	KEY_SPACE	= 0x08,
	KEY_Z			= 0x09
};

// 몇번 플레이어 인지 
enum PLAYER_NUM { Player1, Player2, Player3, Player4, Player5, Player6 };
// 플레이어 애니메이션 번호
enum PLAYER_ANIMATION_NUM { IDLE, RUN };
// 플레이어 상태
enum PLAYER_STATE { NONE, ICE, BREAK };
enum STATE_TYPE { Init, Run, Over };

struct ITEM
{
	bool Hammer;				// 망치
	bool SpecialHammer;	// 특수 망치
	bool Timer;					// 시간 연장
};

struct PLAYER
{
	bool Role; 					// 플레이어들의 역할
	XMFLOAT3 Pos;			// 플레이어 위치
	XMFLOAT4 Dir;				// 방향(쿼터니언)
	byte AnimationNum; 	// 애니메이션 번호
	bool ItemUse;				// 아이템 사용 여부
	ITEM ItemType;				// 아이템 종류
	byte PlayerState;			// 플레이어 상태
	u_short Score;				// 플레이어 점수
};

// 패킷 정의
#pragma pack(1)

// [클라 -> 서버]
struct CS_InitPacket
{
};

struct CS_InGamePacket
{
	byte PlayerID;	// 플레이어 번호
	byte Key;			// 키 버튼
};

// ==========================================

// [서버 -> 클라]
struct SC_InitPacket
{
	XMFLOAT3 ObjPos;	// 오브젝트들 위치
	XMFLOAT4 Dir;		// 방향(쿼터니언)
};

struct SC_InGamePacket
{
	PLAYER		Player_Packet[6];
	byte			RoundCount;			// 몇 라운드인지
	float			Timer;						// 게임 시간
	bool			Bomb;						// 폭탄이 터졌는지
};
#pragma pack()
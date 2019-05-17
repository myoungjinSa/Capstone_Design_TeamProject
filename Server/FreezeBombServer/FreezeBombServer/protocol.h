#pragma once
//#include <iostream>
//#include <windows.h>
//#include <DirectXMath.h>
//#define SERVER_IP "192.168.22.199"
#define SERVER_IP "127.0.0.1"
//#define SERVER_IP "192.168.60.161"
//#define SERVER_IP "192.168.200.103"

using namespace std;
//using namespace DirectX;

constexpr int MAX_USER = 6;


enum ROLE { RUNNER, BOMBER };
enum ITEM { NONEITEM = 0, HAMMER, GOLD_HAMMER, GOLD_TIMER, BOMB };
enum PLAYER_NUM { P1, P2, P3, P4, P5, P6 };						// 몇번 플레이어 인지 
enum PLAYER_STATE { NONESTATE, ICE, BREAK };							// 플레이어 상태
enum STATE_TYPE { Init, Run, Over };
enum MATERIAL { PINK, BROWN, WHITE, BLACK, BLUE, PANDA, ICEMAT };

constexpr int SC_ACCESS_COMPLETE = 1;
constexpr int SC_PUT_PLAYER = 2;
constexpr int SC_MOVE_PLAYER = 3;
constexpr int SC_REMOVE_PLAYER = 4;
constexpr int SC_USE_ITEM = 5;
constexpr int SC_ROLL_CHANGE = 6;
constexpr int SC_ROUND_END = 7;
constexpr int SC_ROUND_START = 8;
constexpr int SC_PLEASE_READY = 9;
constexpr int SC_ACCESS_PLAYER = 10;
constexpr int SC_COMPARE_TIME = 11;
constexpr int SC_STOP_RUN_ANIM = 12;

constexpr int CS_UP_KEY = 0;
constexpr int CS_DOWN_KEY = 1;
constexpr int CS_RIGHT_KEY = 2;
constexpr int CS_LEFT_KEY = 3;
constexpr int CS_UPLEFT_KEY = 4;
constexpr int CS_UPRIGHT_KEY = 5;
constexpr int CS_DOWNLEFT_KEY = 6;
constexpr int CS_DOWNRIGHT_KEY = 7;
constexpr int CS_READY = 8;
constexpr int CS_REQUEST_START = 9;
constexpr int CS_RELEASE_KEY = 10;



constexpr int MAX_ROUND_TIME = 50;

//[클라->서버]

//<< Ready Room 패킷 종류 >>

struct SC_PACKET_ACCESS_COMPLETE
{
	char size;
	char type;
	char myId;
	char hostId;
	char score;				// 플레이어 점수
	char roundCount;		// 몇 라운드인지
	char serverTime;				// 서버 시간
};

struct SC_PACKET_ACCESS_PLAYER
{
	char size;
	char type;
	char id;
};

struct CS_PACKET_READY
{
	char size;
	char type;
	char matID;
};

struct CS_PACKET_REQUEST_START
{
	char size;
	char type;
};

struct SC_PACKET_PLEASE_READY
{
	char size;
	char type;
};

struct SC_PACKET_ROUND_START
{
	char size;
	char type;
	char clientCount;
	char bomberID;
};

struct SC_PACKET_PUT_PLAYER
{
	char size;
	char type;
	char id;
	char score;		// 플레이어 점수
	char matID;			//유저가 원하는 캐릭터는 재질정보가 필요하다.
	float xPos;		// 오브젝트들 위치
	float yPos;
	float zPos;
	float xLook;
	float yLook;
	float zLook;
	float xUp;
	float yUp;
	float zUp;
	float xRight;
	float yRight;
	float zRight;
};

//////////////////////////////////////////////////////

//<< InGame 패킷 종류 >>

//[클라->서버]
struct CS_PACKET_RIGHT_KEY
{
	char size;
	char type;
};

struct CS_PACKET_LEFT_KEY
{
	char size;
	char type;
};

struct CS_PACKET_UP_KEY
{
	char size;
	char type;
};

struct CS_PACKET_DOWN_KEY
{
	char size;
	char type;
};

struct CS_PACKET_BOMBER_TOUCH
{
	char size;
	char type;
	char touchedId;	// 터치한 플레이어 번호
};

struct CS_PACKET_RELEASE_KEY
{
	char size;
	char type;
};
//////////////////////////////////////////////////////

//[서버->클라]
// 폭탄 넘겨주는거 어떻게 구현했는지에 따라 isBomber 변수가 담길 구조체 선정
// 현재 클라는 서버 시간으로 프레임조정하고 있는데 서버 시간을 프레임마다 보내주는게 옳은가?
// -> 건오빠한테 물어보고 timer 변수가 담길 구조체 선정

// isBomber는 키 입력 시 충돌체크하여 역할이 넘어가도록
// animNum 및 animTime은 각 클라가 알아서 하도록
// 몇 라운드 인지는 라운드가 시작할 때 한 번만 보내도 됨
// isBoomed는 게임 시간을 기반으로
struct SC_PACKET_INGAME_PACKET
{
	//char id;
	//char isBomber;
	//char xPos;
	//char yPos;
	//char zPos;
	//char xDir;
	//char yDir;
	//char zDir;
	//char wDir;
	//
	//char animNum;			// 애니메이션 번호
	//char animTime;			// 애니메이션 시간 정보
	//char playerState;		// 플레이어 상태

	//char usedItem;			// 사용되는 아이템 정보
	//char roundCount;		// 몇 라운드인지
	//char timer;				// 서버 시간
	//char isBoomed;			// 폭탄이 터졌는지

	// parameter 생성자
	/*SC_INGAME_PACKET(char _id, char _isBomber, char _xPos, char _yPos, char _zPos, char _xDir, char _yDir, char _zDir, char _wDir,
		char _animNum, char _animTime, char _usedItem, char _playerState, char _roundCount, char _timer, char _isBoomed) :
		id(_id), isBomber(_isBomber), xPos(_xPos), yPos(_yPos), zPos(_zPos), xDir(_xDir), yDir(_yDir), zDir(_zDir), wDir(_wDir),
		animNum(_animNum), animTime(_animTime), usedItem(_usedItem), playerState(_playerState), roundCount(_roundCount), timer(_timer),
		isBoomed(_isBoomed) {};*/
};

// 플레이어 이동 시
struct SC_PACKET_MOVE_PLAYER
{
	char size;
	char type;
	char id;
	float xPos;
	float yPos;
	float zPos;

	//캐릭터의 진행 방향
	float xLook;
	float yLook;
	float zLook;
	float xUp;
	float yUp;
	float zUp;
	float xRight;
	float yRight;
	float zRight;

	//모델 자체의 회전 
	float pitch;
	float yaw;
	float roll;	

	//속도
	float fVelocity;

};

struct SC_PACKET_STOP_RUN_ANIM
{
	char size;
	char type;
	char id;
};

// 플레이어가 아이템 사용 시
struct SC_PACKET_USE_ITEM
{
	char size;
	char type;
	char id;
	char usedItem;			// 사용되는 아이템 정보
};

// 일정 간격으로 서버시간과 클라시간을 비교하기 위해
// 다를 경우 클라시간을 서버시간으로 재설정
struct SC_PACKET_COMPARE_TIME
{
	char size;
	char type;
	float serverTime;				// 서버 시간
};

struct SC_PACKET_ROLL_CHANGE
{
	char size;
	char type;
	char bomberId;
	char normalId;
};

struct SC_PACKET_REMOVE_PLAYER
{
	char size;
	char type;
	char id;
	char hostId;
};

struct SC_PACKET_ROUND_END
{
	char size;
	char type;
	bool isWinner;
};

struct PLAYER
{
	//bool isBomber;        	 // 플레이어들의 역할
	//XMFLOAT3 Pos; 	             // 플레이어 위치
	//XMFLOAT4 Dir;      	// 방향(쿼터니언)
	//byte AnimationNum;    	// 애니메이션 번호
	//float AnimationTime; 	// 애니메이션 시간 정보
	//byte UsedItem;      	// 사용되는 아이템 정보
	//byte PlayerState;  	// 플레이어 상태
};


//////////////////////////////////////////////////////

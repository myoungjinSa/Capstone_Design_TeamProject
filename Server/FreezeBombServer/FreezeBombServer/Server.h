#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "MyInclude.h"
#include "MyDefine.h"
#include "protocol.h"
#include "Terrain.h"
#include "GameTimer.h"

#pragma comment(lib, "Ws2_32.lib")
constexpr int MAX_BUFFER = 1024;

constexpr int SERVER_PORT = 9000;

constexpr float PLAYER_INIT_X_POS = 40;
constexpr float PLAYER_INIT_Y_POS = 0;
constexpr float PLAYER_INIT_Z_POS = 40;

constexpr float PLAYER_INIT_X_LOOK = 0;
constexpr float PLAYER_INIT_Y_LOOK = 0;
constexpr float PLAYER_INIT_Z_LOOK = 0;

constexpr float PLAYER_INIT_X_UP = 0;
constexpr float PLAYER_INIT_Y_UP = 0;
constexpr float PLAYER_INIT_Z_UP = 0;

constexpr float PLAYER_INIT_X_RIGHT = 0;
constexpr float PLAYER_INIT_Y_RIGHT = 0;
constexpr float PLAYER_INIT_Z_RIGHT = 0;

constexpr float FRICTION = 250;
constexpr float MAX_VELOCITY_XZ = 40;
constexpr float MAX_VELOCITY_Y = 400;

constexpr int MAX_WORKER_THREAD = 3;

// Overlapped구조체 확장
struct OVER_EX {
	WSAOVERLAPPED	over;
	WSABUF			dataBuffer;
	char			messageBuffer[MAX_BUFFER];
	bool			is_recv;
};

class SOCKETINFO
{
public:
	// in_use와 x,y에서 data race발생
	// in_use가 true인걸 확인하고 send하려고 하는데 그 이전에 접속 종료해서 false가된다면?
	// 그리고 send 전에 새 플레이어가 id를 재사용한다면? => 엉뚱한 곳에 send가 됨
	// mutex access_lock;
	bool in_use;
	OVER_EX over_ex;
	SOCKET socket;
	// 조립불가한 메모리를 다음번에 조립하기 위한 임시저장소
	char packet_buffer[MAX_BUFFER];
	int prev_size;

	XMFLOAT3 pos;
	XMFLOAT3 look;
	XMFLOAT3 up;
	XMFLOAT3 right;
	XMFLOAT3 velocity;
	float pitch;
	float yaw;
	float roll;
	DWORD direction;
	//속도
	float fVelocity;

	XMFLOAT3 lastRightVector;
	XMFLOAT3 lastLookVector;
	XMFLOAT3 lastUpVector;

	char score;
	char normalItem;
	char specialItem;
	char role;
	char matID;
	bool isReady;
public:
	SOCKETINFO() {
		in_use = false;
		score = 0;
		normalItem = ITEM::NONEITEM;
		specialItem = ITEM::NONEITEM;
		role = ROLE::RUNNER;
		isReady = false;
		ZeroMemory(&over_ex.messageBuffer, sizeof(over_ex.messageBuffer));
		ZeroMemory(&packet_buffer, sizeof(packet_buffer));
		over_ex.dataBuffer.len = MAX_BUFFER;
		over_ex.dataBuffer.buf = over_ex.messageBuffer;
		over_ex.is_recv = true;
	}
};

class Server
{
private:
	SOCKET listenSocket;
	HANDLE iocp;
	mutex myLock;
	// vector로 했을 때 over_ex.messagebuffer에 값이 들어오질 않는다.
	// 배열로 바꾸니 제대로 동작함. 왜? 무슨 차이?
	SOCKETINFO clients[MAX_USER];
	vector<thread> workerThreads;
	CGameTimer gameTimer;
	CHeightMapImage* heightMap;
	XMFLOAT3 gravity;
	int clientCount;
	int readyCount;
	int hostId;
public:
	Server();
	~Server();
public:
	static void AcceptThread(LPVOID arg);
	void AcceptThreadFunc();
	static void WorkerThread(LPVOID arg);
	void WorkerThreadFunc();
public:
	void ProcessPacket(char client, char *packet);
	void SendFunc(char client, void *packet);
	void RecvFunc(char client);
	void ClientDisconnect(char client);
public:
	void SendAccessComplete(char client);
	void SendAccessPlayer(char toClient, char fromClient);
	void SendPutPlayer(char toClient, char fromClient);
	void SendRoundStart(char client);
	void SendPleaseReady(char client);
	void SendMovePlayer(char to,char object);
	void SendRemovePlayer(char toClient, char fromClient);
public:
	void SetVelocityZero(char client);
	void SetPitchYawRollZero(char client);
	void SetClient_Initialize(char client);
	void SetDirection(char client, int key);
	void RotateModel(char client, float x, float y, float z);
	void RotateClientAxisY(char client, float fTimeElapsed);
	void UpdateClientPos(char client, float fTimeElapsed);
	void ProcessClientHeight(char client);
	void ProcessFriction(char client, float& fLength);
public:
	bool InitServer();
	void RunServer();
public:
	void err_quit(const char*);
	void err_display(const char*);
};


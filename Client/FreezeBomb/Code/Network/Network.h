#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>
#include "..\Stdafx\Stdafx.h"
//#include "protocol.h"
#include "../../../../Server/FreezeBombServer/FreezeBombServer/protocol.h"
#include "../Singleton/Singleton.h"
#include <string>


constexpr int SERVER_PORT = 9000;
constexpr int BUF_SIZE = 1024;

class CGameFramework;


#ifdef _WITH_SERVER_
//싱글톤 


class Network : public Singleton<Network>
{
private:
	SOCKET	sock;
	int		myId;
	WSABUF	send_wsabuf;
	char 	send_buffer[BUF_SIZE];
	WSABUF	recv_wsabuf;
	char	recv_buffer[BUF_SIZE];
	char	packet_buffer[BUF_SIZE];
	DWORD	in_packet_size = 0;
	int		saved_packet_size = 0;


//public:
//	SC_PACKET_ACCESS_COMPLETE *pAC = NULL;
//	SC_PACKET_ACCESS_PLAYER *pAP = NULL;
//	SC_PACKET_PUT_PLAYER *pPP = NULL;
//	SC_PACKET_MOVE_PLAYER *pMP = NULL;
//	SC_PACKET_USE_ITEM *pUI = NULL;
//	SC_PACKET_ROLL_CHANGE *pRC = NULL;
//	SC_PACKET_ROUND_END *pRE = NULL;
//	SC_PACKET_REMOVE_PLAYER *pRP = NULL;
//	SC_PACKET_COMPARE_TIME *pCT = NULL;
//	SC_PACKET_ROUND_START *pRS = NULL;
private:
	CS_PACKET_UP_KEY *pUp = NULL;
	CS_PACKET_DOWN_KEY *pDown = NULL;
	CS_PACKET_RIGHT_KEY *pRight = NULL;
	CS_PACKET_LEFT_KEY *pLeft = NULL;
	CS_PACKET_READY *pReady = NULL;
	CS_PACKET_REQUEST_START *pRequestStart = NULL;
	CS_PACKET_RELEASE_KEY *pReleaseKey = NULL;
	CS_PACKET_ANIMATION *pAnimation = NULL;

private:
	//ReadPacket에서 받은 패킷들을 CGameFramework에 전달하기 위한 포인터
	CGameFramework* m_pGameClient{nullptr};
	const  char *   m_ServerIP{ nullptr };
public:
	void err_quit(const char *msg);
	void err_display(const char *msg);
public:
	Network();
	~Network();
public:
	SOCKET getSock();
	bool connectToServer(HWND hWnd );

	//Network클래스도 CGameFramework에 접근가능하게 하기위해 내부 포인터를 갖고있게 함.
	void SetGameFrameworkPtr(HWND hWnd, CGameFramework* client);

	void ReadPacket();
	void SendPacket();
public:
	void SendUpKey();
	void SendUpRightKey();
	void SendUpLeftKey();
	void SendDownKey();
	void SendDownRightKey();
	void SendDownLeftKey();
	void SendRightKey();
	void SendLeftKey();
	void SendReady(int matID);
	void SendReqStart();
	void SendReleaseKey();
	void SendAnimationState(char animNum);
public:
	CS_PACKET_REQUEST_START* GetRS() { return pRequestStart; }
	void SetNullRS() { pRequestStart = NULL; }
	/*SC_PACKET_ACCESS_COMPLETE* GetAC() { return pAC; }
	SC_PACKET_ACCESS_PLAYER* GetAP() { return pAP; }
	SC_PACKET_PUT_PLAYER* GetPP() {return pPP; }
	SC_PACKET_MOVE_PLAYER* GetMP() { return pMP; }
	SC_PACKET_USE_ITEM* GetUI() { return pUI; }
	SC_PACKET_ROLL_CHANGE* GetRC() { return pRC; }
	SC_PACKET_ROUND_END* GetRE() { return pRE; }
	SC_PACKET_REMOVE_PLAYER* GetRP() { return pRP; }
	SC_PACKET_COMPARE_TIME* GetCT() { return pCT; }
	SC_PACKET_ROUND_START* GetRS() { return pRS; }*/

};

#endif
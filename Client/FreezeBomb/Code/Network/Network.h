#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>
#include "..\Stdafx\Stdafx.h"
#include "protocol.h"
#include <string>

#define SERVER_IP "127.0.0.1"
constexpr int SERVER_PORT = 9000;
constexpr int BUF_SIZE = 1024;


class Network
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
private:
	SC_PACKET_ACCESS_COMPLETE *pAC = NULL;
	SC_PACKET_PUT_PLAYER *pPP = NULL;
	SC_PACKET_MOVE_PLAYER *pMP = NULL;
	SC_PACKET_USE_ITEM *pUI = NULL;
	SC_PACKET_ROLL_CHANGE *pRC = NULL;
	SC_PACKET_ROUND_END *pRE = NULL;
	SC_PACKET_REMOVE_PLAYER *pRP = NULL;
	SC_PACKET_COMPARE_TIME *pCT = NULL;
private:
	CS_PACKET_UP_KEY *pUp = NULL;
	CS_PACKET_DOWN_KEY *pDown = NULL;
	CS_PACKET_RIGHT_KEY *pRight = NULL;
	CS_PACKET_LEFT_KEY *pLeft = NULL;
public:
	void err_quit(const char *msg);
	void err_display(const char *msg);
public:
	Network();
	~Network();
public:
	SOCKET getSock();
	void connectToServer(HWND hWnd);
	void ReadPacket();
	void ProcessPacket(char* packet);
	void SendPacket(int data);
};


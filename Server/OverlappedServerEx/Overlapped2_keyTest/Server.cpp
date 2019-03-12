#pragma comment(lib, "ws2_32")
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>

#define SERVERPORT 9000
#define BUFSIZE 512

const static int UP_KEY = 72;
const static int DOWN_KEY = 80;
const static int RIGHT_KEY = 77;
const static int LEFT_KEY = 75;

const static int PLAYER_1 = 0;
const static int PLAYER_2 = 1;
const static int PLAYER_3 = 2;
const static int PLAYER_4 = 3;
const static int PLAYER_5 = 4;
const static int PLAYER_6 = 5;


struct POSITION
{
	POSITION() :x(0), y(0), z(0) {};
	POSITION(int x1, int y1, int z1) :x(x1), y(y1), z(z1) {};
	int x, y, z;
};

class ClientInfo
{
private:
	u_short clientID;
	POSITION pos;
	int key;

public:
	ClientInfo() :clientID(0), pos(0, 0, 0), key(0) {};
	ClientInfo(u_short cid, POSITION p) :clientID(cid), pos(p), key(0) {};
	
	u_short getClientID() { return clientID; }
	void getPos(int& x, int& y, int& z) { x = pos.x; y = pos.y; z = pos.z; }
	int getKey() { return key; }

	void setClientID(u_short cid) { clientID = cid; }
	void setPos(int x, int y, int z) { pos.x = x; pos.y = y; pos.z = z; }
	void setPos(POSITION* p) { pos = *p; }
	void setKey(int k) { key = k; }

public:
	// 복사대입과 복사생성 방지
	ClientInfo(const ClientInfo&) = delete;
	ClientInfo& operator=(const ClientInfo&) = delete;
};

// 소켓 정보 저장을 위한 구조체와 변수
struct SOCKETINFO
{
	SOCKETINFO() { cInfo = new ClientInfo; }
	~SOCKETINFO() { delete cInfo; }
	WSAOVERLAPPED overlapped;
	SOCKET sock;
	ClientInfo* cInfo;
	int recvbytes;
	int sendbytes;
	WSABUF wsabuf;
};


// accept()의 리턴값을 저장할 변수. 두 스레드에서 접근하므로 전역 변수로 선언.
SOCKET client_sock;
// client_sock 변수를 보호하기 위한 이벤트 객체 핸들.
HANDLE hReadEvent, hWriteEvent;

std::vector<SOCKETINFO> vClient;
int clientCount;
CRITICAL_SECTION cs;

// 비동기 입출력 시작과 처리 함수
DWORD WINAPI WorkerThread(LPVOID arg);
void CALLBACK RecvCompletionRoutine(DWORD dwError, DWORD cbTransferred, LPWSAOVERLAPPED lpOverlapped, DWORD dwFlags);
void CALLBACK SendCompletionRoutine(DWORD dwError, DWORD cbTransferred, LPWSAOVERLAPPED lpOverlapped, DWORD dwFlags);
// 오류 출력 함수
void err_quit(const char *msg);
void err_display(const char *msg);
void err_display(const int errcode);

int main(int argc, char *argv[])
{
	InitializeCriticalSection(&cs);
	int retval;
	vClient.reserve(6);
	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET)
		err_quit("socket()");

	// bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR)
		err_quit("bind()");

	// listen()
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR)
		err_quit("listen()");

	// 이벤트 객체 생성
	// CreateEvent(무시, true->수동리셋 / false->자동리셋, true->신호로시작 / false->비신호로시작, 이벤트이름)
	// WorkerThread가 client_sock의 값을 읽었음을 main 스레드에 알림 (읽기 알림)
	hReadEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
	if (hReadEvent == NULL)
		return 1;
	// main 스레드가 client_sock의 값을 변경했음을 alertable wait 상태인 WorkerThread에 알림 (쓰기 알림)
	hWriteEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (hWriteEvent == NULL)
		return 1;

	// 스레드 생성
	HANDLE hThread = CreateThread(NULL, 0, WorkerThread, NULL, 0, NULL);
	if (hThread == NULL)
		return 1;
	CloseHandle(hThread);

	while (1)
	{
		WaitForSingleObject(hReadEvent, INFINITE);
		// accept()
		client_sock = accept(listen_sock, NULL, NULL);
		if (client_sock == INVALID_SOCKET)
		{
			err_display("accept()");
			break;
		}
		SetEvent(hWriteEvent);
	}

	DeleteCriticalSection(&cs);
	// 윈속 종료
	WSACleanup();
	return 0;
}

// 비동기 입출력 처리 함수
DWORD WINAPI WorkerThread(LPVOID arg)
{
	int retval;

	while (1)
	{
		while (1)
		{
			// alertable wait
			DWORD result = WaitForSingleObjectEx(hWriteEvent, INFINITE, TRUE);
			// 새로운 클라이언트가 접속한 경우 -> 루프를 벗어난다.
			if (result == WAIT_OBJECT_0)
				break;
			// WAIT_IO_COMPLETION: (비동기 입출력 작업 -> )완료 루틴 호출 완료 -> 다시 alertable wait 상태에 진입.
			if (result != WAIT_IO_COMPLETION)
				return 1;
		}

		// 접속한 클라이언트 정보 출력
		SOCKADDR_IN clientaddr;
		int addrlen = sizeof(clientaddr);
		getpeername(client_sock, (SOCKADDR *)&clientaddr, &addrlen);
		printf("\n[TCP 서버] 클라이언트 접속: IP 주소 = %s, 포트 번호 = %d\n",
			inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

		// 소켓 정보 구조체 할당과 초기화
		SOCKETINFO *ptr = new SOCKETINFO;
		ptr->cInfo = new ClientInfo((u_short)clientCount, POSITION(0, 0, 0));
		clientCount++;

		if (ptr == NULL)
		{
			printf("[오류] 메모리가 부족합니다!\n");
			return 1;
		}
		ZeroMemory(&ptr->overlapped, sizeof(ptr->overlapped));
		ptr->sock = client_sock;
		// client_sock 변수 값을 읽어가는 즉시 hReadEent 신호 상태로 전환
		SetEvent(hReadEvent);
		ptr->recvbytes = ptr->sendbytes = 0;
		// 괜찮나???? cInfo의 동적할당은 어디서해주지?
		// 프로세스에서 데이터 관리하고 해당 객체 포인터만 넘겨줄까?? 
		ptr->wsabuf.buf = (char *)ptr->cInfo;
		ptr->wsabuf.len = sizeof(ClientInfo);
		// 정보 받아왔으니 client들의 정보를 담는 벡터에 저장
		vClient.emplace_back(*ptr);

		// 비동기 입출력 시작
		DWORD recvbytes;
		DWORD flags = 0;
		retval = WSARecv(ptr->sock, &ptr->wsabuf, 1, &recvbytes, &flags, &ptr->overlapped, RecvCompletionRoutine);
		if (retval == SOCKET_ERROR)
		{
			if (WSAGetLastError() != WSA_IO_PENDING)
			{
				err_display("WSARecv()");
				return 1;
			}
		}

		//DWORD sendbytes;
		//retval = WSASend(ptr->sock, &ptr->wsabuf, 1, &sendbytes, flags, &ptr->overlapped, RecvCompletionRoutine);
		//if (retval == SOCKET_ERROR)
		//{
		//	if (WSAGetLastError() != WSA_IO_PENDING)
		//	{
		//		err_display("WSARecv()");
		//		return 1;
		//	}
		//}
		// delete ptr;
	}
	return 0;
}

// RECV 비동기 입출력 처리 함수(입출력 완료 루틴)
void CALLBACK RecvCompletionRoutine(DWORD dwError, DWORD cbTransferred, LPWSAOVERLAPPED lpOverlapped, DWORD dwFlags)
{
	int retval;

	// 클라이언트 정보 얻기
	SOCKETINFO *ptr = (SOCKETINFO *)lpOverlapped;
	SOCKADDR_IN clientaddr;
	int addrlen = sizeof(clientaddr);
	getpeername(ptr->sock, (SOCKADDR *)&clientaddr, &addrlen);

	// 비동기 입출력 결과 확인
	if (dwError != 0 || cbTransferred == 0)
	{
		if (dwError != 0)
			err_display(dwError);
		closesocket(ptr->sock);
		printf("[TCP 서버] 클라이언트 종료: IP 주소 = %s, 포트 번호 = %d\n",
			inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
		delete ptr;
		return;
	}

	// recv 후처리 코드
	// 들어온 key와 position을 가지고 연산
	int key = 0;
	POSITION tmpPos;

	key = ptr->cInfo->getKey();
	ptr->cInfo->getPos(tmpPos.x, tmpPos.y, tmpPos.z);
	// 왜 key값이 0이 들어가지?
	printf("%d\n", key);

	switch (key)
	{
	case UP_KEY:
		printf("UP_KEY 눌림\n");
		break;
	case DOWN_KEY:
		printf("DOWN_KEY 눌림\n");
		break;
	case RIGHT_KEY:
		printf("RIGHT_KEY 눌림\n");
		break;
	case LEFT_KEY:
		printf("LEFT_KEY 눌림\n");
		break;
	}
	// 데이터 받기
	ZeroMemory(&ptr->overlapped, sizeof(ptr->overlapped));
	ptr->wsabuf.buf = (char *)ptr->cInfo;
	ptr->wsabuf.len = sizeof(ClientInfo);
	
	DWORD recvbytes;
	DWORD flags = 0;
	retval = WSARecv(ptr->sock, &ptr->wsabuf, 1, &recvbytes, &flags, &ptr->overlapped, RecvCompletionRoutine);
	if (retval == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			err_display("WSARecv()");
			return;
		}
	}
	//EnterCriticalSection(&cs);
	//vClient[ptr->cInfo->getClientID()].cInfo->setKey(key);
	//vClient[ptr->cInfo->getClientID()].cInfo->setPos(&tmpPos);
	//LeaveCriticalSection(&cs);
	//
	//// 연산된 내용을 send
	//DWORD sendbytes;
	//retval = WSASend(ptr->sock, &ptr->wsabuf, 1, &sendbytes, 0, &ptr->overlapped, SendCompletionRoutine);
	//if (retval == SOCKET_ERROR)
	//{
	//	if (WSAGetLastError() != WSA_IO_PENDING)
	//	{
	//		err_display("WSASend()");
	//		return;
	//	}
	//}
}

// SEND 비동기 입출력 처리 함수(입출력 완료 루틴)
void CALLBACK SendCompletionRoutine(DWORD dwError, DWORD cbTransferred, LPWSAOVERLAPPED lpOverlapped, DWORD dwFlags)
{
	int retval;

	// 클라이언트 정보 얻기
	SOCKETINFO *ptr = (SOCKETINFO *)lpOverlapped;
	SOCKADDR_IN clientaddr;
	int addrlen = sizeof(clientaddr);
	getpeername(ptr->sock, (SOCKADDR *)&clientaddr, &addrlen);

	// 비동기 입출력 결과 확인
	if (dwError != 0 || cbTransferred == 0)
	{
		if (dwError != 0)
			err_display(dwError);
		closesocket(ptr->sock);
		printf("[TCP 서버] 클라이언트 종료: IP 주소 = %s, 포트 번호 = %d\n",
			inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
		delete ptr;
		return;
	}

	// send 후처리 코드

}

// 소켓 함수 오류 출력 후 종료
void err_quit(const char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

// 소켓 함수 오류 출력
void err_display(const char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (char *)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

void err_display(const int errcode)
{
	LPVOID lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, errcode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&lpMsgBuf, 0, NULL);
	printf("[오류] %s", (char *)lpMsgBuf);
	LocalFree(lpMsgBuf);
}
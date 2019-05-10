#include "Server.h"


Server::Server()
{
	clientCount = 0;
	hostId = -1;
	//clients.reserve(MAX_USER);
	workerThreads.reserve(MAX_WORKER_THREAD);
}


Server::~Server()
{
	//clients.clear();
}

bool Server::InitServer()
{
	clientCount = 0;
	hostId = -1;
	// Winsock Start - windock.dll 로드
	WSADATA WSAData;
	if (WSAStartup(MAKEWORD(2, 2), &WSAData) != 0)
	{
		cout << "Error - Can not load 'winsock.dll' file\n";
		return false;
	}

	// 1. 소켓생성											Overlapped할거면 무조건 WSA_FLAG_OVERLAPPED
	listenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (listenSocket == INVALID_SOCKET)
	{
		cout << "Error - Invalid socket\n";
		return false;
	}

	// 서버정보 객체설정
	SOCKADDR_IN serverAddr;
	memset(&serverAddr, 0, sizeof(SOCKADDR_IN));
	serverAddr.sin_family = PF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	// 2. 소켓설정
	// std의 bind가 호출되므로 소켓의 bind를 불러주기 위해 앞에 ::붙임
	if (::bind(listenSocket, (struct sockaddr*)&serverAddr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
	{
		cout << "Error - Fail bind\n";
		// 6. 소켓종료
		closesocket(listenSocket);
		// Winsock End
		WSACleanup();
		return false;
	}

	// 3. 수신대기열생성
	if (listen(listenSocket, 5) == SOCKET_ERROR)
	{
		cout << "Error - Fail listen\n";
		// 6. 소켓종료
		closesocket(listenSocket);
		// Winsock End
		WSACleanup();
		return false;
	}
	return true;
}

void Server::RunServer()
{
	iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	for (int i = 0; i < MAX_USER; ++i)
	{
		//SOCKETINFO tmpClient;
		clients[i].prev_size = 0;
		clients[i].xPos = PLAYER_INIT_X_POS;
		clients[i].yPos = PLAYER_INIT_Y_POS;
		clients[i].zPos = PLAYER_INIT_Z_POS;
		clients[i].xLook = PLAYER_INIT_X_LOOK;
		clients[i].yLook = PLAYER_INIT_Y_LOOK;
		clients[i].zLook = PLAYER_INIT_Z_LOOK;
		clients[i].xUp = PLAYER_INIT_X_UP;
		clients[i].yUp = PLAYER_INIT_Y_UP;
		clients[i].zUp = PLAYER_INIT_Z_UP;
		clients[i].xRight = PLAYER_INIT_X_RIGHT;
		clients[i].yRight = PLAYER_INIT_Y_RIGHT;
		clients[i].zRight = PLAYER_INIT_Z_RIGHT;
		//clients.emplace_back(tmpClient);
		//printf("Create Client ID: %d, PrevSize: %d, xPos: %d, yPos: %d, zPos: %d, xDir: %d, yDir: %d, zDir: %d\n", i, clients[i].prev_size, clients[i].xPos, clients[i].yPos, clients[i].zPos, clients[i].xDir, clients[i].yDir, clients[i].zDir);
	}
	
	for (int i = 0; i < MAX_WORKER_THREAD; ++i)
		workerThreads.emplace_back(thread{ WorkerThread, (LPVOID)this });
	thread accpetThread{ AcceptThread, (LPVOID)this };
	accpetThread.join();
	for (auto &th : workerThreads)
		th.join();
}

void Server::AcceptThread(LPVOID arg)
{
	Server *pServer = static_cast<Server*>(arg);
	pServer->AcceptThreadFunc();
}

void Server::AcceptThreadFunc()
{
	SOCKADDR_IN clientAddr{};
	int addrLen = sizeof(SOCKADDR_IN);
	memset(&clientAddr, 0, addrLen);
	SOCKET clientSocket{};
	DWORD flags;

	while (1)
	{
		// clientSocket을 비동기식으로 만들기 위해서는 listenSocket이 비동기식이어야 한다.
		clientSocket = accept(listenSocket, (struct sockaddr *)&clientAddr, &addrLen);
		if (clientSocket == INVALID_SOCKET)
		{
			cout << "Error - Accept Failure\n";
			return;
		}

		// id를 0~5까지 사용할 예정
		int new_id = -1;
		for (int i = 0; i < MAX_USER; ++i)
		{
			// i를 인덱스로 하는 값이 몇 개가 있는가?
			if (false == clients[i].in_use)
			{
				new_id = i;
				break;
			}
		}
		if (-1 == new_id)
		{
			cout << "MAX USER overflow\n";
			continue;
		}
		
		///////////////////////////////////// 클라이언트 초기화 정보 수정 위치 /////////////////////////////////////
		clients[new_id].socket = clientSocket;
		if (-1 == hostId)
		{
			hostId = new_id;
			printf("현재 방장은 %d입니다.\n", hostId);
		}
		///////////////////////////////////// 클라이언트 초기화 정보 수정 위치 /////////////////////////////////////
		ZeroMemory(&clients[new_id].over_ex.over, sizeof(clients[new_id].over_ex.over));
		
		flags = 0;

		CreateIoCompletionPort(reinterpret_cast<HANDLE>(clientSocket), iocp, new_id, 0);

		clients[new_id].in_use = true;

		SendAccessComplete(new_id);
		// 기존 유저들에게 이후 접속한 유저들 출력
		for (int i = 0; i < MAX_USER; ++i)
			if (true == clients[i].in_use)
				SendAccessPlayer(i, new_id);

		// 처음 접속한 나에게 기존 유저들 출력
		for (int i = 0; i < MAX_USER; ++i)
		{
			if (false == clients[i].in_use)
				continue;
			if (i == new_id)
				continue;
			SendAccessPlayer(new_id, i);
		}
		++clientCount;
		printf("%d 클라이언트 접속 완료, 현재 클라이언트 수: %d\n", new_id, clientCount);
		RecvFunc(new_id);
	}

	// 6-2. 리슨 소켓종료
	closesocket(listenSocket);

	// Winsock End
	WSACleanup();
}


void Server::RecvFunc(char client)
{
	DWORD flags = 0;

	if (WSARecv(clients[client].socket, &clients[client].over_ex.dataBuffer, 1, NULL, &flags, &(clients[client].over_ex.over), 0))
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			cout << "Error - IO pending Failure\n";
			while (true);
		}
	}
	else {
		// 비동기식으로 돌아가지 않고 동기식으로 돌아갔다는 오류.
		// 키를 동시에 누를 때 발생(ex / UP키와 RIGHT키)
		cout << "Non Overlapped Recv return.\n";
		//while (true);
	}
}

void Server::WorkerThread(LPVOID arg)
{
	Server* pServer = static_cast<Server*>(arg);
	pServer->WorkerThreadFunc();
}

void Server::WorkerThreadFunc()
{
	while (true)
	{
		DWORD io_byte;
		ULONGLONG l_key;
		// 포인터의 포인터를 넘겨줘야
		OVER_EX *over_ex;

		int is_error = GetQueuedCompletionStatus(iocp, &io_byte, &l_key, reinterpret_cast<LPWSAOVERLAPPED *>(&over_ex), INFINITE);

		if (0 == is_error)
		{
			int err_no = WSAGetLastError();
			if (64 == err_no)
			{
				ClientDisconnect(l_key);
				continue;
			}
			else
				err_display("GQCS : ");
		}

		if (0 == io_byte)
		{
			ClientDisconnect(l_key);
			continue;
		}

		char key = static_cast<char>(l_key);
		if (true == over_ex->is_recv)
		{
			// RECV 처리
			wcout << "Packet from Client: " << (int)key << "\n";
			// 패킷조립
			// 남은 크기
			int rest = io_byte;
			// 실제 버퍼
			char *ptr = over_ex->messageBuffer;
			char packet_size = 0;

			// 패킷 사이즈 알아내기 (중반부터 가능)
			if (0 < clients[key].prev_size)
				packet_size = clients[key].packet_buffer[0];

			while (0 < rest) {
				if (0 == packet_size) packet_size = ptr[0];	// ptr[0]이 지금부터 처리할 패킷
				// 패킷 처리하려면 얼마나 더 받아야 하는가?
				// 이전에 받은 조립되지 않은 패킷이 있을 수 있으니 prev_size 빼주기
				int required = packet_size - clients[key].prev_size;
				if (required <= rest) {
					// 패킷 만들 수 있는 경우

					// 앞에 와있던 데이터가 저장되어있을 수 있으니 그 뒤에 저장되도록 prev_size를 더해준다.
					memcpy(clients[key].packet_buffer + clients[key].prev_size, ptr, required);
					ProcessPacket(key, clients[key].packet_buffer);
					rest -= required;
					ptr += required;
					// 이미 계산됐기 때문에 다음 패킷을 처리할 수 있도록 0
					packet_size = 0;
					clients[key].prev_size = 0;
				}
				else {
					// 패킷 만들 수 없는 경우
					memcpy(clients[key].packet_buffer + clients[key].prev_size, ptr, rest);
					rest = 0;
					clients[key].prev_size += rest;
				}
			}

			RecvFunc(key);
		}
		else
		{
			// SEND 처리
			delete over_ex;
		}
	}
}

void Server::ProcessPacket(char client, char *packet)
{
	float x = clients[client].xPos;
	float y = clients[client].yPos;
	float z = clients[client].zPos;

	// 0번은 사이즈, 1번이 패킷타입
	switch (packet[1])
	{
	case CS_UP_KEY:
		printf("Press UP Key ID: %d\n", client);
		y += 12.25f;
		clients[client].xPos = x;
		clients[client].yPos = y;
		clients[client].zPos = z;
		printf("Move Player ID: %d\tx: %f, y: %f, z: %f\n", client, x, y, z);

		SendMovePlayer(client);
		break;
	case CS_DOWN_KEY:
		printf("Press DOWN Key ID: %d\n", client);
		y -= 12.25f;
		clients[client].xPos = x;
		clients[client].yPos = y;
		clients[client].zPos = z;
		printf("Move Player ID: %d\tx: %f, y: %f, z: %f\n", client, x, y, z);

		SendMovePlayer(client);
		break;
	case CS_LEFT_KEY:
		printf("Press LEFT Key ID: %d\n", client);
		x -= 12.25f;
		clients[client].xPos = x;
		clients[client].yPos = y;
		clients[client].zPos = z;
		printf("Move Player ID: %d\tx: %f, y: %f, z: %f\n", client, x, y, z);

		SendMovePlayer(client);
		break;
	case CS_RIGHT_KEY:
		printf("Press RIGHT Key ID: %d\n", client);
		x += 12.25f;
		clients[client].xPos = x;
		clients[client].yPos = y;
		clients[client].zPos = z;
		printf("Move Player ID: %d\tx: %f, y: %f, z: %f\n", client, x, y, z);

		SendMovePlayer(client);
		break;
	case CS_READY:
		printf("전체 클라 수: %d\n", clientCount);
		printf("Ready한 클라 수: %d\n", ++readyCount);
		clients[client].isReady = true;
		clients[client].matID = packet[2];	// matID
		printf("Recv matID : %d\n", clients[client].matID);
		break;
	case CS_REQUEST_START:
		if (clientCount == readyCount)
		{
			for (int i = 0; i < MAX_USER; ++i)
			{
				if (true == clients[i].in_use)
				{
					for (int j = 0; j < MAX_USER; ++j)
					{
						if (true == clients[j].in_use)
							SendPutPlayer(i, j);
					}
					SendRoundStart(i);
				}
			}
			printf("Round Start\n");
		}
		else
		{
			for (int i = 0; i < MAX_USER; ++i)
			{
				if (true == clients[i].in_use)
					SendPleaseReady(i);
			}
			printf("Please Ready\n");
		}
		break;
	default:
		wcout << L"정의되지 않은 패킷 도착 오류!!\n";
		while (true);
	}
	
}	

void Server::SendFunc(char client, void *packet)
{
	char *p = reinterpret_cast<char *>(packet);
	OVER_EX *ov = new OVER_EX;
	ov->dataBuffer.len = p[0];
	ov->dataBuffer.buf = ov->messageBuffer;
	ov->is_recv = false;
	memcpy(ov->messageBuffer, p, p[0]);
	ZeroMemory(&ov->over, sizeof(ov->over));
	int error = WSASend(clients[client].socket, &ov->dataBuffer, 1, 0, 0, &ov->over, NULL);
	if (0 != error)
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			cout << "Error - IO pending Failure\n";
			while (true);
		}
	}
	else {
		// 비동기식으로 돌아가지 않고 동기식으로 돌아갔다는 오류.
		//cout << "Non Overlapped Send return.\n";
		//while (true);
	}
}

void Server::SendAccessComplete(char client)
{
	SC_PACKET_ACCESS_COMPLETE packet;
	packet.myId = client;
	packet.hostId = hostId;
	packet.size = sizeof(packet);
	packet.type = SC_ACCESS_COMPLETE;

	SendFunc(client, &packet);
}

void Server::SendAccessPlayer(char toClient, char fromClient)
{
	SC_PACKET_ACCESS_PLAYER packet;
	packet.id = fromClient;
	packet.size = sizeof(packet);
	packet.type = SC_ACCESS_PLAYER;

	SendFunc(toClient, &packet);
}

void Server::SendPutPlayer(char toClient, char fromClient)
{
	SC_PACKET_PUT_PLAYER packet;
	packet.id = fromClient;
	packet.size = sizeof(packet);
	packet.type = SC_PUT_PLAYER;
	packet.xPos = clients[fromClient].xPos;
	packet.yPos = clients[fromClient].yPos;
	packet.zPos = clients[fromClient].zPos;
	packet.xLook = clients[fromClient].xLook;
	packet.yLook = clients[fromClient].yLook;
	packet.zLook = clients[fromClient].zLook;
	packet.xUp = clients[fromClient].xUp;
	packet.yUp = clients[fromClient].yUp;
	packet.zUp = clients[fromClient].zUp;
	packet.xRight = clients[fromClient].xRight;
	packet.yRight = clients[fromClient].yRight;
	packet.zRight = clients[fromClient].zRight;
	packet.matID = clients[fromClient].matID;
	printf("Send matID : %d\n", packet.matID);
	SendFunc(toClient, &packet);
}

void Server::SendRoundStart(char client)
{
	SC_PACKET_ROUND_START packet;
	packet.size = sizeof(packet);
	packet.clientCount = clientCount;
	packet.type = SC_ROUND_START;

	SendFunc(client, &packet);
}

void Server::SendPleaseReady(char client)
{
	SC_PACKET_PLEASE_READY packet;
	packet.size = sizeof(packet);
	packet.type = SC_PLEASE_READY;

	SendFunc(client, &packet);

}

void Server::SendMovePlayer(char client)
{
	SC_PACKET_MOVE_PLAYER packet;
	packet.id = client;
	packet.size = sizeof(packet);
	packet.type = SC_MOVE_PLAYER;
	packet.xPos = clients[client].xPos;
	packet.yPos = clients[client].yPos;
	packet.zPos = clients[client].zPos;

	SendFunc(client, &packet);
}

void Server::SendRemovePlayer(char toClient, char fromClient)
{
	SC_PACKET_REMOVE_PLAYER packet;
	packet.id = fromClient;
	packet.hostId = hostId;
	packet.size = sizeof(packet);
	packet.type = SC_REMOVE_PLAYER;

	SendFunc(toClient, &packet);
}

void Server::ClientDisconnect(char client)
{
	
	clients[client].in_use = false;
	if (clients[client].isReady)
	{
		readyCount--;
		clients[client].isReady = false;
	}
	if (hostId == client)
	{
		for (int i = 0; i < MAX_USER; ++i)
		{
			if (true == clients[i].in_use)
			{
				hostId = i;
				printf("현재 방장은 %d입니다.\n", hostId);
				break;
			}
		}
	}
	for (int i = 0; i < MAX_USER; ++i)
	{
		if (false == clients[i].in_use)
			continue;
		if (i == client)
			continue;
		SendRemovePlayer(i, client);
	}
	closesocket(clients[client].socket);
	clientCount--;
	printf("%d 클라이언트 접속 종료, 현재 클라이언트 수: %d\n", (int)client, clientCount);
}

void Server::err_quit(const char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage
	(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0,
		NULL
	);

	MessageBox(NULL, (LPCTSTR)lpMsgBuf, (LPCTSTR)msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	//exit(1);
}

void Server::err_display(const char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage
	(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0,
		NULL
	);
	cout << msg << (char*)lpMsgBuf << endl;

	LocalFree(lpMsgBuf);
}
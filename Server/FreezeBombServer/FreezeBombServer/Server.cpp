#include "Server.h"


Server::Server()
{
	round = 0;
	recent_pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	player_pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	roundCurrTime = 0;
	clientCount = 0;
	hostId = -1;
	freezeCnt = 0;
	//clients.reserve(MAX_USER);
	
	for (int i = 0; i < MAX_ROUND; ++i)
	{
		goldHammerCnt[i] = 0;
		goldTimerCnt[i] = 0;
		normalHammerCnt[i] = 0;
	}

	workerThreads.reserve(MAX_WORKER_THREAD);
}


Server::~Server()
{
	//clients.clear();
	if (heightMap)
		delete heightMap;
	for (int i = 0; i < MAX_ROUND; ++i)
	{
		if (false == objects[i].empty())
			objects[i].clear();
	}
}

bool Server::InitServer()
{
	setlocale(LC_ALL, "korean");

	LoadMapObjectInfo();

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

void Server::LoadMapObjectInfo()
{
	for (int j = 0; j < MAX_ROUND; ++j)
	{
		string filename;
		filename = "../Resource/Position/Surrounding/Round" + to_string(j) + ".bin";
		ifstream in(filename, ios::binary);

		if (!in)
		{
			cout <<filename << " - 바이너리 파일 없음" << endl;
			return;
		}

		size_t nReads = 0;
		XMFLOAT3 tmp;

		// 맵 오브젝트 개수
		int nObjects = 0;
		in.read(reinterpret_cast<char*>(&nObjects), sizeof(int));

		for (int i = 0; i < nObjects; ++i)
		{
			MAPOBJECT* pMapObjectInfo = new MAPOBJECT;
			// 모델 이름 문자열 길이 저장
			in.read(reinterpret_cast<char*>(&nReads), sizeof(size_t));

			// 길이 + 1만큼 자원 할당
			char* p = new char[nReads + 1];
			in.read(p, sizeof(char) * nReads);
			p[nReads] = '\0';
			//  모델 이름 저장
			pMapObjectInfo->name = p;
			delete[] p;

			// Position 문자열 길이 저장
			in.read(reinterpret_cast<char*>(&nReads), sizeof(size_t));
			p = new char[nReads + 1];
			in.read(p, sizeof(char)*nReads);
			p[nReads] = '\0';
			delete[] p;

			// Position x, y, z값 저장
			in.read(reinterpret_cast<char*>(&pMapObjectInfo->pos.x), sizeof(float));
			in.read(reinterpret_cast<char*>(&pMapObjectInfo->pos.y), sizeof(float));
			in.read(reinterpret_cast<char*>(&pMapObjectInfo->pos.z), sizeof(float));

			// Look 문자열 길이 저장
			in.read(reinterpret_cast<char*>(&nReads), sizeof(size_t));
			p = new char[nReads + 1];
			in.read(p, sizeof(char)*nReads);
			p[nReads] = '\0';
			delete[] p;

			// <Look> x, y, z값 저장
			in.read(reinterpret_cast<char*>(&tmp.x), sizeof(float));
			in.read(reinterpret_cast<char*>(&tmp.y), sizeof(float));
			in.read(reinterpret_cast<char*>(&tmp.z), sizeof(float));

			// Up 문자열 길이 저장
			in.read(reinterpret_cast<char*>(&nReads), sizeof(size_t));
			p = new char[nReads + 1];
			in.read(p, sizeof(char)*nReads);
			p[nReads] = '\0';
			delete[] p;

			// <Up> x, y, z값 저장
			in.read(reinterpret_cast<char*>(&tmp.x), sizeof(float));
			in.read(reinterpret_cast<char*>(&tmp.y), sizeof(float));
			in.read(reinterpret_cast<char*>(&tmp.z), sizeof(float));

			// Right 문자열 길이 저장
			in.read(reinterpret_cast<char*>(&nReads), sizeof(size_t));
			p = new char[nReads + 1];
			in.read(p, sizeof(char)*nReads);
			p[nReads] = '\0';
			delete[] p;

			// <Right> x, y, z값 저장
			in.read(reinterpret_cast<char*>(&tmp.x), sizeof(float));
			in.read(reinterpret_cast<char*>(&tmp.y), sizeof(float));
			in.read(reinterpret_cast<char*>(&tmp.z), sizeof(float));

			pMapObjectInfo->pos.y = 0.f;

			if (0 == strcmp(pMapObjectInfo->name.c_str(), "GoldTimer"))
			{
				goldTimers[j].emplace_back(*pMapObjectInfo);
				++goldTimerCnt[j];
			}
			else if (0 == strcmp(pMapObjectInfo->name.c_str(), "Hammer"))
			{
				if (goldHammerCnt[j] < GOLD_HAMMER_COUNT)
				{
					goldHammers[j].emplace_back(*pMapObjectInfo);
					++goldHammerCnt[j];
				}
				else
				{
					NormalHammers[j].emplace_back(*pMapObjectInfo);
					++normalHammerCnt[j];
				}
			}
			else
				objects[j].emplace_back(*pMapObjectInfo);
		}
		in.close();

		
	}
}

void Server::RunServer()
{
	gameTimer.Start();
	iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	for (int i = 0; i < MAX_USER; ++i)
	{
		//SOCKETINFO tmpClient;
		clients[i].prev_size = 0;
		clients[i].pos.x = PLAYER_INIT_X_POS;
		clients[i].pos.y = PLAYER_INIT_Y_POS;
		clients[i].pos.z = PLAYER_INIT_Z_POS;
		clients[i].look.x = PLAYER_INIT_X_LOOK;
		clients[i].look.y = PLAYER_INIT_Y_LOOK;
		clients[i].look.z = PLAYER_INIT_Z_LOOK;
		clients[i].up.x = PLAYER_INIT_X_UP;
		clients[i].up.y = PLAYER_INIT_Y_UP;
		clients[i].up.z = PLAYER_INIT_Z_UP;
		clients[i].right.x = PLAYER_INIT_X_RIGHT;
		clients[i].right.y = PLAYER_INIT_Y_RIGHT;
		clients[i].right.z = PLAYER_INIT_Z_RIGHT;
		//clients.emplace_back(tmpClient);
		//printf("Create Client ID: %d, PrevSize: %d, xPos: %d, yPos: %d, zPos: %d, xDir: %d, yDir: %d, zDir: %d\n", i, clients[i].prev_size, clients[i].xPos, clients[i].yPos, clients[i].zPos, clients[i].xDir, clients[i].yDir, clients[i].zDir);
	}
	heightMap = new CHeightMapImage("../../../Client/FreezeBomb/Resource/Textures/Terrain/Terrain.raw", 256, 256, XMFLOAT3(2.0f, 1.0f, 2.0f));
	
	gameTimer.Tick(60.0f);
	for (int i = 0; i < MAX_WORKER_THREAD; ++i)
		workerThreads.emplace_back(thread{ WorkerThread, (LPVOID)this });
	thread accpetThread{ AcceptThread, (LPVOID)this };
	thread timerThread{ TimerThread,(LPVOID)this };
	accpetThread.join();
	timerThread.join();
	for (auto &th : workerThreads)
		th.join();
}

void Server::add_timer(int obj_id, EVENT_TYPE et, chrono::high_resolution_clock::time_point start_time)
{
	// lock에서 오류남.. 수정 필요
	timer_l.lock();
	timer_queue.emplace(EVENT_ST{ obj_id, et,  start_time });
	timer_l.unlock();
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
		SetClient_Initialize(new_id);
		///////////////////////////////////// 클라이언트 초기화 정보 수정 위치 /////////////////////////////////////
		ZeroMemory(&clients[new_id].over_ex.over, sizeof(clients[new_id].over_ex.over));
		
		flags = 0;
		CreateIoCompletionPort(reinterpret_cast<HANDLE>(clientSocket), iocp, new_id, 0);

		clients[new_id].in_use = true;
		clients[new_id].velocity = XMFLOAT3(0.0f, 0.0f, 1.0f);
		clients[new_id].gameState = GS_LOBBY;

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
		gLock.lock();
		++clientCount;
		gLock.unlock();
		printf("%d 클라이언트 접속 완료, 현재 클라이언트 수: %d\n", new_id, clientCount);
		RecvFunc(new_id);
	}

	// 6-2. 리슨 소켓종료
	closesocket(listenSocket);

	// Winsock End
	WSACleanup();
}

void Server::TimerThread(LPVOID arg)
{
	Server *pServer = static_cast<Server*>(arg);
	pServer->TimerThreadFunc();
}

void Server::TimerThreadFunc()
{
	while (1)
	{
		this_thread::sleep_for(10ms);

		while (1)
		{
			timer_l.lock();
			if (true == timer_queue.empty())
			{
				timer_l.unlock();
				break;
			}
		
			EVENT_ST ev = timer_queue.top();
	
			// 시간 됐나 확인
			if (ev.start_time > chrono::high_resolution_clock::now())
			{
				timer_l.unlock();
				break;
			}
		
			timer_queue.pop();
			timer_l.unlock();
			OVER_EX *over_ex = new OVER_EX;
			over_ex->event_t = ev.type;
			PostQueuedCompletionStatus(iocp, 1, ev.obj_id, &over_ex->over);
		}

	}
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
		//cout << "Non Overlapped Recv return.\n";
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
		if (EV_RECV == over_ex->event_t)
		{
			// RECV 처리
			//wcout << "Packet from Client: " << (int)key << "\n";
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
		else if(EV_SEND == over_ex->event_t)
		{
			// SEND 처리
			delete over_ex;
		}
		else if (EV_COUNT == over_ex->event_t)
		{
			roundTime_l.lock();
			unsigned short time = --roundCurrTime;
			roundTime_l.unlock();

			//서버 시간 현재 (남은 시간)
		//	printf("RoundCurrentTime : %d", roundCurrTime);
			for (int i = 0; i < MAX_USER; ++i)
			{
				if (true == clients[i].in_use)
					SendCompareTime(i, roundCurrTime);
			}

			// 라운드 종료 시
			roundTime_l.lock();
			if (roundCurrTime <= 0)
			{
				roundTime_l.unlock();
				if (round >= MAX_ROUND)
				{
					for (int i = 0; i < MAX_USER; ++i)
					{
						if (true == clients[i].in_use)
							SendRoundEnd(i);
					}
				}
				else
				{
					++round;

					// 새 라운드 시작 시 초기화
					StartTimer();

					roundTime_l.lock();
					unsigned short time = roundCurrTime;
					roundTime_l.unlock();
					for (int i = 0; i < MAX_USER; ++i)
					{
						if (true == clients[i].in_use)
						{

							for (int j = 0; j < MAX_USER; ++j)
							{
								if (true == clients[j].in_use)
								{

									clients[j].pos.x = (100.0f*j + 50);
									clients[j].pos.z = 100.0f;

									SendMovePlayer(i, j);

								}
							}
							SendRoundStart(i, time);
						}
					}
					//여기서도 add_timer(-1, EV_COUNT, chrono::high_resolution_clock::now() + 1s) 를 호출해주지 않으면 
					//다음 라운드로 넘어가서 시간이 감소하지 않음, 확인 바람 - 명진
				}
			}
			else
			{
				roundTime_l.unlock();
				add_timer(-1, EV_COUNT, chrono::high_resolution_clock::now() + 1s);
			}
		}
		else if(EV_COOLTIME == over_ex->event_t)		//cooltime 계산
		{
			coolTime_l.lock();
			changeCoolTime--;
			coolTime_l.unlock();

			printf("쿨타임:%d\n", changeCoolTime);
			coolTime_l.lock();
			if(changeCoolTime<=0)
			{
			
				changeCoolTime = 0;
				coolTime_l.unlock();
			}
			else
			{
				coolTime_l.unlock();
				add_timer(-1, EV_COOLTIME, chrono::high_resolution_clock::now() + 1s);
			}

		}
		else
		{
			cout << "Unknown Event\n";
			while (true);
		}
	}
}

void Server::PickBomber()
{
	default_random_engine dre;
	uniform_int_distribution<int> uid(0, clientCount + 1);
	while (true)
	{
		bomberID = uid(dre);
		if (true == clients[bomberID].in_use)
			break;
	}
	cout << "술래 ID:" << bomberID << "\n";
}


void Server::StartTimer()
{
	roundTime_l.lock();
	roundCurrTime = MAX_ROUND_TIME;
	roundTime_l.unlock();
}

void Server::ResetTimer()
{
	roundTime_l.lock();
	roundCurrTime = 0;
	roundTime_l.unlock();
}

void Server::ProcessPacket(char client, char *packet)
{
	float x = clients[client].pos.x;
	float y = clients[client].pos.y;
	float z = clients[client].pos.z;

	DWORD tmpDir;

	// 0번은 사이즈, 1번이 패킷타입
	switch (packet[1])
	{
	case CS_UP_KEY:
	case CS_DOWN_KEY:
	case CS_LEFT_KEY:
	case CS_RIGHT_KEY:
	case CS_UPLEFT_KEY:
	case CS_UPRIGHT_KEY:
	case CS_DOWNLEFT_KEY:
	case CS_DOWNRIGHT_KEY:
	{
		SetDirection(client, packet[1]);

		float time = gameTimer.GetTimeElapsed();

		UpdateClientPos(client, time);


		//printf("Move Player ID: %d\tx: %f, y: %f, z: %f\n", client, x, y, z);
		for (int i = 0; i < MAX_USER; ++i)
		{
			if (clients[i].in_use == true)
			{
				SendMovePlayer(i, client);
				//한번 MovePacket을 보내고 난후 
				//pitch,yaw,roll은 다시 0으로 바꿔줘야함.
				//그렇지 않을경우 뱅글뱅글 돌게됨.
				SetPitchYawRollZero(i);

				//Idle 동작으로 변하게 하려면 
				//SetVelocityZero(i);
			}
		}
		break;
	}
	case CS_OBJECT_COLLISION:
	{	
		CS_PACKET_OBJECT_COLLISION *p = reinterpret_cast<CS_PACKET_OBJECT_COLLISION *>(packet);


		// 서버에서 플레이어와 해당 맵 오브젝트 사이의 거리(위치+부피)를 측정하여 검증 후 broadcast
		float dist = sqrt(pow(clients[client].pos.x - objects[round][p->objId].pos.x, 2) +
			pow(clients[client].pos.y - objects[round][p->objId].pos.y, 2) +
			pow(clients[client].pos.z - objects[round][p->objId].pos.z, 2));
		
		recent_objects = objects[round][p->objId]; //최근에 부딪힌 오브젝트를 저장한다.
		recent_pos = objects[round][p->objId].pos;

		clients[client].collision = CL_SURROUNDING;

		break;
	}
	case CS_NOT_COLLISION:
	{
		CS_PACKET_NOT_COLLISION *p = reinterpret_cast<CS_PACKET_NOT_COLLISION *>(packet);

		
		//최근 검사한 recent_object와만 거리 검사를 실시
		float dist = sqrt(pow(clients[client].pos.x - recent_objects.pos.x, 2) +
			pow(clients[client].pos.y - recent_objects.pos.y, 2) +
			pow(clients[client].pos.z - recent_objects.pos.z, 2));

		


		clients[client].collision = CL_NONE;


		break;
	}//주변 Surrounding 객체들과 플레이어들의 위치 판별검사를 다르게 하기 위해 경우의 수를 나눠야한다.
	case CS_PLAYER_COLLISION:
	{
		CS_PACKET_PLAYER_COLLISION* p = reinterpret_cast<CS_PACKET_PLAYER_COLLISION*>(packet);

		float dist = sqrt(pow(clients[client].pos.x - clients[p->playerID].pos.x, 2) +
		pow(clients[client].pos.y - clients[p->playerID].pos.y, 2) +
			pow(clients[client].pos.z - clients[p->playerID].pos.z, 2));

		recent_player = p->playerID;
		player_pos = clients[recent_player].pos;

		clients[client].collision = CL_PLAYER;

		break;
	}
	case CS_BOMBER_TOUCH:
	{
		CS_PACKET_BOMBER_TOUCH* p = reinterpret_cast<CS_PACKET_BOMBER_TOUCH*>(packet);

		if (client != bomberID)
			break;
		
		if (changeCoolTime > 0 || changeCoolTime < 0 )		//쿨타임이 아직 남아있으면 RoleChange를 하지 않는다.
			break;

		float dist = sqrt(pow(clients[client].pos.x - clients[p->touchId].pos.x, 2) +
		pow(clients[client].pos.y - clients[p->touchId].pos.y, 2) +
			pow(clients[client].pos.z - clients[p->touchId].pos.z, 2));

		bomberID = p->touchId;

		coolTime_l.lock();
		changeCoolTime = COOLTIME;
		if (changeCoolTime == COOLTIME) 
		{
			coolTime_l.unlock();
			add_timer(-1, EV_COOLTIME, chrono::high_resolution_clock::now() + 1s);
		}
		else
		{
			coolTime_l.unlock();
		}

		for(int i=0;i<MAX_USER;++i)
		{
			if (true == clients[i].in_use)
				SendChangeBomber(i, bomberID,client);
		}
		

		break;
	}

	case CS_ANIMATION_INFO:		//클라가 애니메이션이 변경되었을때 패킷을 서버에게 보내고.
	{							//서버는 그 패킷을 받아서 다른 클라이언트에게 해당 애니메이션 정보를 보낸다.
		CS_PACKET_ANIMATION* p = reinterpret_cast<CS_PACKET_ANIMATION*>(packet);

		SetAnimationState(client, p->animation);
		for (int i = 0; i < MAX_USER; ++i)
		{
			if (client == i)
				continue;
			if (clients[i].in_use == true)
			{
				SendPlayerAnimation(i, client);
			}
		}

		break;
	}
	case CS_RELEASE_KEY:
		if (clients[client].fVelocity > 0)
		{
			for (int i = 0; i < MAX_USER; ++i)
			{
				if (true == clients[i].in_use)
					SendStopRunAnim(i, client);
			}
		}
		break;
	case CS_READY:
		printf("전체 클라 수: %d\n", clientCount);
		// 클라가 엔터누르고 F5누를때마다 CS_READY 패킷이 날아온다면 ++readyCount는 clientCount보다 증가하게 되고 
		// 아래 CS_REQUEST_START안에 if(clientCount<= readyCount) 안으로 들어가지 않는 현상 발생
		gLock.lock();
		++readyCount;
		gLock.unlock();

		printf("Ready한 클라 수: %d\n", readyCount);

		clients[client].isReady = true;
		clients[client].matID = packet[2];	// matID
		for(int i=0;i<MAX_USER;++i)
		{
			if (clients[i].in_use == true)
				SendReadyStatePacket(i, client);
		}
		//printf("Recv matID : %d\n", clients[client].matID);
		break;
	case CS_UNREADY:
	{
		gLock.lock();
		--readyCount;
		gLock.unlock();


		printf("Ready한 클라 수: %d\n", readyCount);

		clients[client].isReady = false;

		for(int i=0;i<MAX_USER;++i)
		{
			if (clients[i].in_use == true)
				SendUnReadyStatePacket(i, client);
		}
		break;
	}
	case CS_NICKNAME_INFO:
	{
		CS_PACKET_NICKNAME* p = reinterpret_cast<CS_PACKET_NICKNAME*>(packet);
		
	
		strcpy_s(clients[client].nickname, sizeof(p->name), p->name);
		for (int i = 0; i < MAX_USER; ++i)
			if (true == clients[i].in_use)
				SendClientLobbyIn(i, client,clients[client].nickname);

		// 처음 접속한 나에게 기존 유저들 출력
		for (int i = 0; i < MAX_USER; ++i)
		{
			if (false == clients[i].in_use)
				continue;
			if (i == client)
				continue;
			SendClientLobbyIn(client, i,clients[i].nickname);
		}
		cout << clients[client].nickname << endl;
		cout <<(int)p->id << endl;
		break;
	}
	case CS_CHATTING:
	{
		CS_PACKET_CHATTING* p = reinterpret_cast<CS_PACKET_CHATTING*>(packet);

		for(int i =0; i<MAX_USER ;++i)
		{
			if(true == clients[i].in_use)
			{
				cout << sizeof(p->chatting) << endl;
				SendChattinPacket(i, client, p->chatting);
			}
		}
		cout << p->chatting << endl;


		break;
	}
	case CS_REQUEST_START:
		if (clientCount <= readyCount)
		{
			PickBomber();
			
			
			for(int i=0;i<MAX_USER;++i)
			{
				if (clients[i].in_use == true)
				{
					clients[i].gameState = GS_INGAME;
				}
			}
			// 라운드 시작시간 set
			StartTimer();
		
			roundTime_l.lock();
			unsigned short time = roundCurrTime;
			roundTime_l.unlock();
			for(int i=0; i< MAX_USER ;++i)
			{
				if(true == clients[i].in_use)
				{
				
					for (int j = 0; j < MAX_USER; ++j)
					{
						if (true == clients[j].in_use)
						{
							
							clients[j].pos.x = (100.0f*j + 50);
							clients[j].pos.z = 100.0f;
							
							SendPutPlayer(i, j);
							
						}
					}
					SendRoundStart(i,time);
				}
			}
			add_timer(-1, EV_COUNT, chrono::high_resolution_clock::now() + 1s);

			
			printf("Round Start\n");
		}
		else
		{
			//이 부분 READYCOUNT 보다
			for (int i = 0; i < MAX_USER; ++i)
			{
				if (true == clients[i].in_use)
					SendPleaseReady(i);
			}
			printf("Please Ready\n");
		}
		break;
	case CS_GET_ITEM:
	{
		CS_PACKET_GET_ITEM *p = reinterpret_cast<CS_PACKET_GET_ITEM *>(packet);

		string tmps = p->itemIndex;

		char *token = NULL;
		int itemIdx = 0;
		float dist = 0.f;

		token = strtok(p->itemIndex, " ");
		//cout << p->itemIndex << "\n";
		if (strcmp(token, "GoldTimer") == 0)
		{
			token = strtok(NULL, " ");
			itemIdx = atoi(token);

			dist = sqrt(pow(clients[client].pos.x - goldTimers[round][itemIdx].pos.x, 2) +
				pow(clients[client].pos.y - goldTimers[round][itemIdx].pos.y, 2) +
				pow(clients[client].pos.z - goldTimers[round][itemIdx].pos.z, 2));

			// 거리 체크는 적당히 넓도록 그렇지 않으면 
			// 클라는 아이템과 충돌해서 월드상에 아이템을 지우는데
			// 인벤토리에는 아이템 등록이 안됨.
			if (dist <= 50)
			{
				clients[client].specialItem = itemIdx;
				for (int i = 0; i < MAX_USER; ++i)
				{
					if (true == clients[i].in_use)
						SendGetItem(i, client, tmps);
				}
			}

			//cout << dist << "\n";
		}
		else if (strcmp(token, "GoldHammer") == 0)
		{
			token = strtok(NULL, " ");
			itemIdx = atoi(token);

			dist = sqrt(pow(clients[client].pos.x - goldHammers[round][itemIdx].pos.x, 2) +
				pow(clients[client].pos.y - goldHammers[round][itemIdx].pos.y, 2) +
				pow(clients[client].pos.z - goldHammers[round][itemIdx].pos.z, 2));

			// 거리 체크는 적당히 넓도록 그렇지 않으면 
			// 클라는 아이템과 충돌해서 월드상에 아이템을 지우는데
			// 인벤토리에는 아이템 등록이 안됨.
			if (dist <= 50)
			{
				clients[client].specialItem = itemIdx;
				for (int i = 0; i < MAX_USER; ++i)
				{
					if (true == clients[i].in_use)
						SendGetItem(i, client, tmps);
				}
			}

			//cout << dist << "\n";
		}
		else
		{
			token = strtok(NULL, " ");
			itemIdx = atoi(token);

			dist = sqrt(pow(clients[client].pos.x - NormalHammers[round][itemIdx].pos.x, 2) +
				pow(clients[client].pos.y - NormalHammers[round][itemIdx].pos.y, 2) +
				pow(clients[client].pos.z - NormalHammers[round][itemIdx].pos.z, 2));

			// 거리 체크는 적당히 넓도록 그렇지 않으면 
			// 클라는 아이템과 충돌해서 월드상에 아이템을 지우는데
			// 인벤토리에는 아이템 등록이 안됨.
			if (dist <= 50)		
			{
				clients[client].normalItem = itemIdx;
				for (int i = 0; i < MAX_USER; ++i)
				{
					if (true == clients[i].in_use)
						SendGetItem(i, client, tmps);
				}
			}
			//cout << dist << "\n";
		}

		break;
	}
	case CS_USEITEM:
	{
		CS_PACKET_USE_ITEM *p = reinterpret_cast<CS_PACKET_USE_ITEM *>(packet);


		switch(p->usedItem)
		{
		case NORMALHAMMER:
		{
			break;
		}
		case GOLD_HAMMER:
		{
			break;
		}
		case GOLD_TIMER:
		{
			//실제 이 클라이언트가 이 아이템을 들고 있는지 검사를 해야함.
			//이 클라이언트가 도망자인지 검사해야함.

			roundTime_l.lock();
			unsigned short time = roundCurrTime += 60;
			roundTime_l.unlock();


			for (int i = 0; i < MAX_USER; ++i)
			{
				if (clients[i].in_use == true)
				{
					SendCompareTime(i,time);
					SendUseItem(i, client, ITEM::GOLD_TIMER, client);
				}
			}
			break;
		}
		case EMPTY:
		{
			break;
		}
		default:
			printf("미정의 패킷\n");
			break;
		}

		break;
	}
	case CS_FREEZE:
	{
		// 1. 현재 Freeze를 요청한 id클라가 도망자인지를 판단해야함.
		// 2. 현재 모든 도망자가 Freeze상태인지를 확인해야함.
		// 3. 모든 도망자가 Freeze가 아니고 해당 클라가 도망자가 맞다면 SC_FREEZE를 각 클라들에게 알려줌
		if (client == bomberID)		//술래라면 얼음을 할 수 없다.
			break;

		if (freezeCnt >= MAX_FREEZE_COUNT)	//최대 얼음할 수 있는 도망자 수를 넘으면 얼음을 하게 할 수 없다.
			break;

		++freezeCnt;

		for(int i=0;i<MAX_USER;++i)
		{
			if (clients[i].in_use == true)
				SendFreeze(i, client);
		}
		

		break;
	}
	case CS_RELEASE_FREEZE:
	{
		if (client == bomberID)		//술래라면 얼음을 할 수 없다.
			break;

		if (freezeCnt <= 0)
			break;

		--freezeCnt;

		for(int i=0;i<MAX_USER;++i)
		{
			if (clients[i].in_use == true)
				SendReleaseFreeze(i, client);
		}
		break;
	}
	case CS_BOMB_EXPLOSION:
	{
		if (client != bomberID)		//술래가 아닌 다른 클라이언트가 보냈다면 무시한다.
			break;
		//Bomber가 아닌 다른 나머지 클라이언트에게 폭탄이 터진것을 알림
		for(int i=0;i<MAX_USER;++i)
		{
			if (clients[i].in_use == false)
				continue;
			if (i == bomberID)
				continue;
			SendBombExplosion(i, bomberID);
		}

		break;
	}
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
	ov->event_t = EV_SEND;
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

void Server::SendChangeHostID(char toClient,char hostID)
{
	SC_PACKET_CHANGE_HOST packet;
	packet.type = SC_CHANGE_HOST_ID;
	packet.size = sizeof(packet);
	packet.hostID = hostID;

	SendFunc(toClient, &packet);
}

void Server::SendClientLobbyIn(char toClient,char fromClient,char* name)
{
	SC_PACKET_LOBBY_IN packet;
	packet.id = fromClient;
	packet.size = sizeof(packet);
	packet.type = SC_CLIENT_LOBBY_IN;
	packet.client_state.isReady = false;
	strcpy_s(packet.client_state.name, name);

	SendFunc(toClient, &packet);
}
void Server::SendClientLobbyOut(char toClient,char fromClient)
{
	SC_PACKET_LOBBY_OUT packet;
	packet.id = fromClient;
	packet.size = sizeof(packet);
	packet.type = SC_CLIENT_LOBBY_OUT;
	
	SendFunc(toClient, &packet);
}

void Server::SendChattinPacket(char toClient,char fromClient,char* message)
{
	SC_PACKET_CHATTING packet;
	ZeroMemory(&packet, sizeof(SC_PACKET_CHATTING));
	packet.id = fromClient;
	packet.size = sizeof(packet);
	packet.type = SC_CHATTING;
	
	strcpy_s(packet.message,MAX_CHATTING_LENGTH, message);

	SendFunc(toClient, &packet);

}
void Server::SendPutPlayer(char toClient, char fromClient)
{
	SC_PACKET_PUT_PLAYER packet;
	packet.id = fromClient;
	packet.size = sizeof(packet);
	packet.type = SC_PUT_PLAYER;
	packet.xPos = clients[fromClient].pos.x;
	packet.yPos = clients[fromClient].pos.y;
	packet.zPos = clients[fromClient].pos.z;
	packet.xLook = clients[fromClient].look.x;
	packet.yLook = clients[fromClient].look.y;
	packet.zLook = clients[fromClient].look.z;
	packet.xUp = clients[fromClient].up.x;
	packet.yUp = clients[fromClient].up.y;
	packet.zUp = clients[fromClient].up.z;
	packet.xRight = clients[fromClient].right.x;
	packet.yRight = clients[fromClient].right.y;
	packet.zRight = clients[fromClient].right.z;
	packet.matID = clients[fromClient].matID;

	SendFunc(toClient, &packet);
}

void Server::SendRoundStart(char client,unsigned short& t)
{
	SC_PACKET_ROUND_START packet;
	packet.bomberID = bomberID;
	packet.startTime = t;
	packet.round = round;
	packet.goldTimerCnt = goldTimerCnt[round];
	packet.goldHammerCnt = goldHammerCnt[round];
	packet.hammerCnt = normalHammerCnt[round];
	packet.clientCount = clientCount;
	packet.size = sizeof(packet);
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


//무브 플레이어 패킷이 현재는 움직임을 요청한 클라이언트에게만
//전송하는 구조여서 이부분 바뀌어야 할듯 - 명진

//TO ,Object
void Server::SendMovePlayer(char to,char client)
{
	SC_PACKET_MOVE_PLAYER packet;
	packet.id = client;
	packet.size = sizeof(packet);
	packet.type = SC_MOVE_PLAYER;

	packet.xPos = clients[client].pos.x;
	packet.yPos = clients[client].pos.y;
	packet.zPos = clients[client].pos.z;
	//플레이어의 진행 방향 정보 ,look,right,up
	packet.xLook = clients[client].look.x;
	packet.yLook = clients[client].look.y;
	packet.zLook = clients[client].look.z;
	packet.xRight= clients[client].right.x;
	packet.yRight= clients[client].right.y;
	packet.zRight= clients[client].right.z;
	packet.xUp = clients[client].up.x;
	packet.yUp = clients[client].up.y;
	packet.zUp = clients[client].up.z;

	//플레이어 모델의 실제 회전정보
	packet.pitch = clients[client].pitch;
	packet.yaw = clients[client].yaw;
	packet.roll = clients[client].roll;

	packet.fVelocity = clients[client].fVelocity;

	SendFunc(to, &packet);
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

void Server::SendRoundEnd(char client)
{
	SC_PACKET_ROUND_END packet;
	packet.isWinner = true;
	if (client == bomberID)
		packet.isWinner = false;
	packet.size = sizeof(packet);
	packet.type = SC_ROUND_END;

	SendFunc(client, &packet);
}

void Server::SendCompareTime(char client,unsigned short& time)
{
	SC_PACKET_COMPARE_TIME packet;
	packet.serverTime = time;
	packet.size = sizeof(packet);
	packet.type = SC_COMPARE_TIME;

	SendFunc(client, &packet);
}

void Server::SendPlayerAnimation(char toClient,char fromClient)
{
	SC_PACKET_PLAYER_ANIMATION packet;
	packet.id = fromClient;
	packet.size = sizeof(packet);
	packet.type = SC_ANIMATION_INFO;
	packet.animation = clients[fromClient].animation;


	SendFunc(toClient, &packet);
}
void Server::SendStopRunAnim(char toClient, char fromClient)
{
	SC_PACKET_STOP_RUN_ANIM packet;
	packet.id = fromClient;
	packet.size = sizeof(packet);
	packet.type = SC_STOP_RUN_ANIM;

	SendFunc(toClient, &packet);
}

void Server::SendReadyStatePacket(char toClient,char fromClient)
{
	SC_PACKET_READY_STATE packet;

	packet.id = fromClient;
	packet.size = sizeof(packet);
	packet.type = SC_READY_STATE;

	SendFunc(toClient, &packet);

}
void Server::SendUnReadyStatePacket(char toClient,char fromClient)
{
	SC_PACKET_UNREADY_STATE packet;

	packet.id = fromClient;
	packet.size = sizeof(packet);
	packet.type = SC_UNREADY_STATE;

	SendFunc(toClient, &packet);
}

void Server::SendChangeBomber(char toClient, char bomberID,char runnerID)
{
	SC_PACKET_ROLE_CHANGE packet;

	packet.type = SC_ROLE_CHANGE;
	packet.size = sizeof(packet);
	packet.bomberId = bomberID;
	packet.runnerId = runnerID;

	SendFunc(toClient, &packet);

}

void Server::SendGetItem(char toClient, char fromClient, string& itemIndex)
{
	SC_PACKET_GET_ITEM packet;

	packet.id = fromClient;
	packet.size = sizeof(SC_PACKET_GET_ITEM);
	packet.type = SC_GET_ITEM;
	ZeroMemory(packet.itemIndex, MAX_ITEM_NAME_LENGTH);
	strncpy(packet.itemIndex, itemIndex.c_str(), itemIndex.length());
	for (int i = 0; i < MAX_ITEM_NAME_LENGTH; ++i)
		cout << packet.itemIndex[i];
	cout << "\n";
	SendFunc(toClient, &packet);
}

void Server::SendUseItem(char toClient, char fromClient, char usedItem, char targetClient)
{
	SC_PACKET_USE_ITEM packet;

	packet.id = fromClient;
	packet.target = targetClient;
	packet.size = sizeof(packet);
	packet.type = SC_USE_ITEM;

	SendFunc(toClient, &packet);
}

void Server::SendFreeze(char toClient,char fromClient)
{
	SC_PACKET_FREEZE packet;

	packet.size = sizeof(packet);
	packet.type = SC_FREEZE;
	packet.id = fromClient;

	SendFunc(toClient, &packet);

}

void Server::SendReleaseFreeze(char toClient,char fromClient)
{
	SC_PACKET_RELEASE_FREEZE packet;

	packet.size = sizeof(packet);
	packet.type = SC_RELEASE_FREEZE;
	packet.id = fromClient;

	SendFunc(toClient, &packet);
}

void Server::SendBombExplosion(char toClient,char fromClient)
{
	SC_PACKET_BOMB_EXPLOSION packet;

	packet.size = sizeof(packet);
	packet.type = SC_BOMB_EXPLOSION;
	packet.bomberId = fromClient;

	SendFunc(toClient, &packet);
}
void Server::ClientDisconnect(char client)
{
	clients[client].in_use = false;
	if (clients[client].isReady)
	{
		gLock.lock();
		readyCount--;
		gLock.unlock();
		clients[client].isReady = false;
	}
	if (hostId == client)
	{
		for (int i = 0; i < MAX_USER; ++i)
		{
			if (true == clients[i].in_use)
			{
				hostId = i;
				SendChangeHostID(i, hostId);
				printf("현재 방장은 %d입니다.\n", hostId);
				break;
			}
		}
	}
	gLock.lock();
	clientCount--;
	gLock.unlock();
	
	switch(clients[client].gameState)
	{
	case GS_LOBBY:
	{
		for (int i = 0; i < MAX_USER; ++i)
		{
			if (false == clients[i].in_use)
				continue;
			if (i == client)
				continue;
			SendClientLobbyOut(i, client);
		}
		break;
	}
	case GS_INGAME:
	{
		for (int i = 0; i < MAX_USER; ++i)
		{
			if (false == clients[i].in_use)
				continue;
			if (i == client)
				continue;
			SendRemovePlayer(i, client);
		}
		break;
	}
	}

	
	closesocket(clients[client].socket);

	printf("%d 클라이언트 접속 종료, 현재 클라이언트 수: %d\n", (int)client, clientCount);
}

void Server::SetAnimationState(char client, char animationNum)
{
	clients[client].animation = animationNum;

}
void Server::SetDirection(char client, int key)
{
	DWORD tmpDir = 0;
	switch (key)
	{
	case CS_UP_KEY:
		tmpDir |= DIR_FORWARD;
		break;
	case CS_DOWN_KEY:
		tmpDir |= DIR_BACKWARD;
		break;
	case CS_RIGHT_KEY:
		tmpDir |= DIR_RIGHT;
		break;
	case CS_LEFT_KEY:
		tmpDir |= DIR_LEFT;
		break;
	case CS_UPLEFT_KEY:
		tmpDir |= DIR_FORWARDLEFT;
		break;
	case CS_UPRIGHT_KEY:
		tmpDir |= DIR_FORWARDRIGHT;
		break;
	case CS_DOWNLEFT_KEY:
		tmpDir |= DIR_BACKLEFT;
		break;

	case CS_DOWNRIGHT_KEY:
		tmpDir |= DIR_BACKRIGHT;
		break;
	}
	clients[client].direction = tmpDir;
}

void Server::SetVelocityZero(char client)
{
	clients[client].fVelocity = 0.0f;
}
void Server::SetClient_Initialize(char client)
{
	clients[client].look = XMFLOAT3(0.0f, 0.0f, 1.0f);
	clients[client].right = XMFLOAT3(1.0f, 0.0f, 0.0f);
	clients[client].up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	clients[client].collision = CL_NONE;
	
}


void Server::SetPitchYawRollZero(char client)
{
	if (clients[client].pitch > 0.0f)
		clients[client].pitch = 0.0f;
	if (clients[client].yaw > 0.0f)
		clients[client].yaw = 0.0f;
	if (clients[client].roll > 0.0f)
		clients[client].roll = 0.0f;
	
}
void Server::UpdateClientPos(char client, float fTimeElapsed)
{
	if (clients[client].direction == DIR_FORWARD)
	{
		clients[client].velocity = Vector3::Add(clients[client].velocity, clients[client].look, VELOCITY);

	}
	if (clients[client].direction == DIR_BACKWARD)
	{
		clients[client].velocity = Vector3::Add(clients[client].velocity, clients[client].look, -VELOCITY);
	}
	if (clients[client].direction == DIR_LEFT 
		|| clients[client].direction == DIR_RIGHT
		|| clients[client].direction == DIR_FORWARDRIGHT
		|| clients[client].direction == DIR_FORWARDLEFT
		|| clients[client].direction == DIR_BACKRIGHT
		|| clients[client].direction == DIR_BACKLEFT)
	{
		RotateClientAxisY(client, fTimeElapsed);
	}

	//clients[client].velocity = Vector3::Add(clients[client].velocity, gravity);//gravity가 초기화가 안되서 쓰레기값?
	//	cout << clients[client].velocity.x<<","<<clients[client].velocity.y<<","<<clients[client].velocity.z << "\n";
	float fLength = sqrtf(clients[client].velocity.x * clients[client].velocity.x + clients[client].velocity.z * clients[client].velocity.z);
	
	
	if (fLength > MAX_VELOCITY_XZ)
	{
		clients[client].velocity.x *= (MAX_VELOCITY_XZ / fLength);
		clients[client].velocity.z *= (MAX_VELOCITY_XZ / fLength);
	}

	float fLengthY = sqrtf(clients[client].velocity.y * clients[client].velocity.y);


	XMFLOAT3 xmf3Velocity = Vector3::ScalarProduct(clients[client].velocity, fTimeElapsed, false);
	
	switch (clients[client].collision)
	{
	case CL_NONE:
	{
		break;
	}
	case CL_SURROUNDING:
	{
	
		XMFLOAT3 xmf3CollisionDir = Vector3::Subtract(recent_pos, clients[client].pos);
		xmf3CollisionDir = Vector3::ScalarProduct(xmf3CollisionDir, VELOCITY*0.3f );
		clients[client].velocity = XMFLOAT3(-xmf3CollisionDir.x, -xmf3CollisionDir.y, -xmf3CollisionDir.z);
		
		break;
	}
	case CL_PLAYER:
	{
		XMFLOAT3 xmf3CollisionDir = Vector3::Subtract(player_pos,clients[client].pos);
		xmf3CollisionDir = Vector3::ScalarProduct(xmf3CollisionDir, 0.5f );
		clients[client].velocity = XMFLOAT3(-xmf3CollisionDir.x, -xmf3CollisionDir.y, -xmf3CollisionDir.z);
		
		break;
	}
	default :
		cout << "알수 없는 객체와 충돌\n";
		break;
	}


	clients[client].pos = Vector3::Add(clients[client].pos, clients[client].velocity);

	//현재 높이값은 0으로 정해져있어서 필요없는 함수 
	//ProcessClientHeight(client);

	ProcessFriction(client, fLength);


	//속도를 클라에게 보내주어 클라에서 기본적인 rUn,Backward,애니메이션을 결정하게 하기 위해.
	clients[client].fVelocity = fLength;

	//cout << clients[client].fVelocity << endl;

}

void Server::RotateClientAxisY(char client, float fTimeElapsed)
{
	XMFLOAT3& xmf3Look = clients[client].look;
	XMFLOAT3& xmf3Right = clients[client].right;
	XMFLOAT3& xmf3Up = clients[client].up;

	clients[client].lastLookVector = xmf3Look;
	clients[client].lastRightVector = xmf3Right;
	clients[client].lastUpVector = xmf3Up;

		

	if (clients[client].direction & DIR_RIGHT)
	{
		float fDotProduct = Vector3::DotProduct(xmf3Look, xmf3Right);

		float fAngle = ::IsEqual(fDotProduct, 1.0f) ? 0.0f : ((fDotProduct > 1.0f) ? XMConvertToDegrees(acos(fDotProduct)) : 90.0f);

		XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&xmf3Up), XMConvertToRadians(fAngle*fTimeElapsed*0.3f));
		xmf3Look = Vector3::TransformNormal(xmf3Look, xmmtxRotate);
		xmf3Right = Vector3::TransformNormal(xmf3Right, xmmtxRotate);

		float cxDelta = xmf3Right.x - clients[client].lastRightVector.x;
		float cyDelta = xmf3Up.y - clients[client].lastUpVector.y;
		float czDelta = xmf3Look.z - clients[client].lastLookVector.z;


		RotateModel(client, 0.0f, fAngle*fTimeElapsed * 0.3f, 0.0f);
		
	}
	if (clients[client].direction & DIR_LEFT)
	{
		float fDotProduct = Vector3::DotProduct(xmf3Look, xmf3Right);

		float fAngle = ::IsEqual(fDotProduct, 1.0f) ? 0.0f : ((fDotProduct > 1.0f) ? XMConvertToDegrees(acos(fDotProduct)) : 90.0f);

		XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&xmf3Up), XMConvertToRadians(-(fAngle*fTimeElapsed*0.3f)));
		xmf3Look = Vector3::TransformNormal(xmf3Look, xmmtxRotate);
		xmf3Right = Vector3::TransformNormal(xmf3Right, xmmtxRotate);

		float czDelta = xmf3Look.z - clients[client].lastLookVector.z;

		RotateModel(client, 0.0f, -fAngle * fTimeElapsed * 0.3f, 0.0f);
		
	}
	if (clients[client].direction & DIR_FORWARDRIGHT )
	{	
		
		clients[client].velocity = Vector3::Add(clients[client].velocity, clients[client].look, 0.001f);
	}
	if (clients[client].direction & DIR_FORWARDLEFT)
	{
		
		clients[client].velocity = Vector3::Add(clients[client].velocity, clients[client].look, 0.001f);
	}
	if (clients[client].direction & DIR_BACKRIGHT)
	{
		
		clients[client].velocity = Vector3::Add(clients[client].velocity, clients[client].look, -0.001f);
	}
	if (clients[client].direction & DIR_BACKLEFT)
	{
		clients[client].velocity = Vector3::Add(clients[client].velocity, clients[client].look, -0.001f);
	}

	xmf3Look = Vector3::Normalize(xmf3Look);
	xmf3Right = Vector3::CrossProduct(xmf3Up, xmf3Look, true);
	xmf3Up = Vector3::CrossProduct(xmf3Look, xmf3Right, true);

	clients[client].look = xmf3Look;
	clients[client].right = xmf3Right;
	clients[client].up = xmf3Up;

}

void Server::RotateModel(char client, float x, float y, float z)
{
	if (x != 0.0f)
	{
		clients[client].pitch += x;
		if (clients[client].pitch > +89.0f) { x -= (clients[client].pitch - 89.0f); clients[client].pitch = +89.0f; }
		if (clients[client].pitch < -89.0f) { x -= (clients[client].pitch + 89.0f); clients[client].pitch = -89.0f; }
	}
	if (y != 0.0f)
	{
		clients[client].yaw += y;
		if (clients[client].yaw > 360.0f) clients[client].yaw -= 360.0f;
		if (clients[client].yaw < 0.0f) clients[client].yaw += 360.0f;
	}
	if (z != 0.0f)
	{
		clients[client].roll += z;
		if (clients[client].roll > +20.0f) { z -= (clients[client].roll - 20.0f); clients[client].roll = +20.0f; }
		if (clients[client].roll < -20.0f) { z -= (clients[client].roll + 20.0f); clients[client].roll = -20.0f; }
	}
}

void Server::ProcessClientHeight(char client)
{
	int z = (int)(clients[client].pos.z / heightMap->GetScale().z);
	bool bReverseQuad = ((z % 2) != 0);

	float fHeight = heightMap->GetHeight(clients[client].pos.x, clients[client].pos.z, bReverseQuad);
	if (clients[client].pos.y < fHeight)
	{
		clients[client].velocity.y = 0.0f;
		
		clients[client].pos.y = fHeight;
		
	}
}

void Server::ProcessFriction(char client, float& fLength)
{
	
	fLength = Vector3::Length(clients[client].velocity);
	float fDeclaration = (FRICTION * gameTimer.GetTimeElapsed());

	if (fDeclaration > fLength)
	{
		fDeclaration = fLength;
		clients[client].velocity = Vector3::Add(clients[client].velocity, Vector3::ScalarProduct(clients[client].velocity, -fDeclaration, true));
	}
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
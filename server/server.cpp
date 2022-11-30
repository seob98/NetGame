#include "protocol.h"

#define SERVERPORT 9000
#define BUFSIZE    4096
#define MAX_PLAYER 4

int horzBlockCnt = 15;
int vertBlockCnt = 13;
int ballonID{};
int explodedBallon[30]{};

CLIENTINFO players[MAX_PLAYER];
SC_GAMEINFO player_data{};	// SC_GAMEINFO를 배열에서 단일 변수로 변환
CS_EVENT event_data[4]{};
SC_PLAYERUPDATE update_data[4]{};

SC_BALLONBOMBEVENT update_ballondata{};

HANDLE recvEvent[4], updateEvent, uThread;
std::vector<CBlock> map;
std::vector<CObstacle> obstacles;
std::vector<CBallon> ballons{};
std::vector<CWaterStream> waterstreams{};


CRITICAL_SECTION cs;
int cur_player = 0;

DWORD WINAPI UpdateThread(LPVOID arg);

void Map_Init()
{
	// 맵 바닥 설치 ( 포지션 계산을 위해 )
	for (int i = 0; i < 13; ++i)
	{
		for (int j = 0; j < 15; ++j)
		{
			map.emplace_back(POINT{ 40 + j * 40, 60 + i * 40 }, 20, i * 15 + j);
		}
	}

	// player_data의 itemtype과 blocktype의 값을 채우는 함수

	int map_range = INDEX_MAPEND - INDEX_MAPSTART;
	for (int i = 0; i < map_range; i++) {
		if (rand() % 10 <= 7)	// 70% 확률로 벽, 30%확률로 공간
		{
			if (rand() % 10 <= 3)
			{
				player_data.blockType[i] = 1;		//돌
				obstacles.emplace_back(map[i + 30].GetPos(), map[i + 30].GetIndex(), map, true);
			}
			else
			{
				player_data.blockType[i] = 0;		//나무블럭
				obstacles.emplace_back(map[i + 30].GetPos(), map[i + 30].GetIndex(), map, false);
			}
		}
		else
			player_data.blockType[i] = -1;
	}

	for (int i = 0; i < MAX_ITEM_CNT; i++)
	{
		player_data.itemType[i].type = rand() % 7;
		player_data.itemType[i].pos = rand() % (map_range)+INDEX_MAPSTART;
	}

}


unsigned short Player_Create(SOCKET sock)
{
	// 게임 접속시 플레이어 상태 대기 상태
	CPlayer::STATE p_state = CPlayer::STATE::IDLE;
	POINT pos{};
	if (cur_player == 0) {
		pos = map[0].GetPos();
	}
	else if (cur_player == 1) {
		pos = map[14].GetPos();
	}
	else if (cur_player == 2) {
		pos = map[180].GetPos();
	}
	else if (cur_player == 3) {
		pos = map[194].GetPos();
	}

	players[cur_player].sock = sock;
	players[cur_player].ID = cur_player;
	players[cur_player].player.SetPosX(pos.x);
	players[cur_player].player.SetPosY(pos.y);
	cur_player++;

	if (cur_player == 4) {
		uThread = CreateThread(NULL, 0, UpdateThread,
			NULL, 0, NULL);
		if (uThread == NULL) {}
		else { CloseHandle(uThread); }
	}

	return players[cur_player - 1].ID;

}

// 클라이언트와 데이터 통신
DWORD WINAPI RecvThread(LPVOID arg)
{
	int retval{};
	SOCKET client_sock = (SOCKET)arg;
	struct sockaddr_in clientaddr;
	char addr[INET_ADDRSTRLEN];
	int addrlen{};
	int len{};
	char buf[BUFSIZE];

	// 클라이언트 정보 얻기
	addrlen = sizeof(clientaddr);
	getpeername(client_sock, (struct sockaddr*)&clientaddr, &addrlen);
	inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));

	int myID = Player_Create(client_sock);
	if (myID == -1)
		printf("생성 실패\n");

	// 클라이언트 초기 정보 보내주기
	player_data.ID = myID;
	player_data.gameStart = 0;
	player_data.currentPlayerCnt = cur_player;
	if (player_data.ID % 2 == 0)
		player_data.teamId = BAZZI;
	else
		player_data.teamId = DAO;

	retval = send(client_sock, (char*)&player_data, sizeof(SC_GAMEINFO), 0);
	printf("\n초기 게임 정보 전송\n");

	if (retval == SOCKET_ERROR) {
		err_display("send()");
		return NULL;
	}

	if (player_data.ID > 0) {
		for (int i = 0; i < player_data.ID; i++) {
			retval = send(players[i].sock, (char*)&player_data, sizeof(SC_GAMEINFO), 0);
			printf("\n기존 유저에게 내 게임 정보 전송\n");
		}
	}

	while (1) {
		retval = recv(players[myID].sock, buf, sizeof(CS_EVENT), MSG_WAITALL);
		if (retval == SOCKET_ERROR) {
			err_display("recv()");
			break;
		}
		CS_EVENT* event = (CS_EVENT*)buf;
		EnterCriticalSection(&cs);
		

		event_data[myID].Index = event->Index;
		event_data[myID].moving = event->moving;
		event_data[myID].State = event->State;
		event_data[myID].setBallon = event->setBallon;

		//if (event->setBallon)
		//	printf("ID: %d, Index: (%d, %d), State: %d, setBallon: %d\n", event->ID,
		//		players[myID].player.GetPos().x, players[myID].player.GetPos().y,
		//		event->State, event->setBallon);
		SetEvent(recvEvent[myID]);
		LeaveCriticalSection(&cs);
		//WaitForSingleObject(updateEvent, INFINITE);
	}
	return 0;
}

void PlayerMove()
{
	for (int i = 0; i < MAX_PLAYER; i++) 
	{
		if (event_data[i].moving)
		{
			players[i].player.Move(map, event_data[i].State);
		}
		else
			players[i].player.SetMoving(false);
	}
}

void PlayerMapCollisionCheck(/*std::vector<CBlock>& TILES*/)
{
	for (int i = 0; i < MAX_PLAYER; i++)
	{
		if (event_data[i].moving)
		{
			players[i].player.CheckCollisionMap(map);
		}
	}
}

void PlayerBallonCollisionCheck()
{
	std::vector<CPlayer*> ptPlayers;
	for (auto& p : players)
		ptPlayers.emplace_back(&(p.player));

	for (auto& ballon : ballons)
	{
		ballon.CheckPlayerOut(ptPlayers);
		ballon.CheckCollision(ptPlayers);
		//ballon.UpdateFrame();
	}
}

void PlayerWaterstreamCollisionCheck()
{
	std::vector<CPlayer*> ptPlayers;
	for (auto& p : players)
		ptPlayers.emplace_back(&(p.player));
	
	for(auto& p : ptPlayers)
	{
		p->CheckCollisionWaterStreams(waterstreams);
	}
}

void BallonUpdate()
{
	//memset(explodedBallon, -1, sizeof(int) * 30);
	memset(update_ballondata.explodedBomb, -1, sizeof(int) * 30);

	for (auto& ballon : ballons)
		ballon.Update(ballons, waterstreams, map, obstacles, horzBlockCnt, vertBlockCnt);

	int cnt{};

	for (int i = 0; i < ballons.size();)
	{
		if (ballons[i].getExplode())
		{
			//explodedBallon[cnt] = ballons[i].GetID();
			update_ballondata.explodedBomb[cnt] = ballons[i].GetID();
			++cnt;

			//std::cout << "ID : " << ballons[i].GetID() << "풍선 터짐" << std::endl;

			ballons.erase(ballons.begin() + i);
		}
		else
			++i;
	}

	if (cnt > 0)
	{
		std::cout << "배열값" << std::endl;
		//for (auto t : explodedBallon)
		for (auto t : update_ballondata.explodedBomb)
		{
			std::cout << "ID : " << t << "풍선 터짐" << std::endl;
		}
	}
}

void PlaceBallon()
{
	for (int i = 0; i < MAX_PLAYER; i++)
	{
		players[i].player.setSpaceButton(event_data[i].setBallon);
		if (event_data[i].setBallon == true)
		{
			int placed = players[i].player.SetupBallon(map, ballons, true, ballonID);

			if (placed)
			{
				update_data[i].setBallon;
				std::cout << i << "번 updateData.setballon에 true값을 지정" << std::endl;
			}

		}
	}
}

void WaterStreamUpdate()
{
	//물줄기 업데이트
	for (int i = 0; i < waterstreams.size(); ++i)
	{
		waterstreams[i].Update();
	}
	for (int i = 0; i < waterstreams.size();)
	{
		if (waterstreams[i].GetDead())
			waterstreams.erase(waterstreams.begin() + i);
		else
			++i;
	}
}

void ObstacleUpdate()
{
	//장애물 처리 (컨테이너 비우기)
	for (int i = 0; i < obstacles.size(); ++i)
	{
		obstacles[i].CheckExplosionRange(map);
		//obstacles[i].makeItem(ITEMS);
	}
	for (int i = 0; i < obstacles.size();)
	{
		if (obstacles[i].GetDead())
			obstacles.erase(obstacles.begin() + i);
		else
			++i;
	}
}


DWORD WINAPI UpdateThread(LPVOID arg)
{
	int retval;
#pragma region 서버객체 업데이트
	while (1) {
		//WaitForMultipleObjects(4, recvEvent, TRUE, 33);
		WaitForMultipleObjects(4, recvEvent, FALSE, INFINITE);

		//for (int i = 0; i < 4; ++i)
		//{
		//	if (event_data[i].setBallon)
		//		std::cout << i << "번 클라의 setBallon은 true" << std::endl;
		//}

		PlayerMove();
		PlayerWaterstreamCollisionCheck();
		PlayerMapCollisionCheck();
		PlayerBallonCollisionCheck();

		BallonUpdate();
		WaterStreamUpdate();

		PlaceBallon();

		ObstacleUpdate();

		//WaterStreamUpdate();

		// 업데이트 보내기
		for (int i = 0; i < 4; i++) {
			update_data[i].moving = players[i].player.isMoving();
			update_data[i].playerDir = players[i].player.GetDir();
			update_data[i].state = players[i].player.Get_State();
			update_data[i].pt = players[i].player.GetPos();
			update_data[i].setBallon = players[i].player.spaceButton();
			//if (update_data[i].setBallon)
			//	std::cout << i << "번 클라의 setBallon = true를 베포" << std::endl;
		}

		for (auto& clients : players)
		{
			retval = send(clients.sock, (char*)&update_data, sizeof(SC_PLAYERUPDATE) * 4, 0);
			if (retval == SOCKET_ERROR) {
				err_display("send()");
				break;
			}


			retval = send(clients.sock, (char*)&update_ballondata, sizeof(SC_BALLONBOMBEVENT), 0);
			if (retval == SOCKET_ERROR) {
				err_display("send()");
				break;
			}
		}

		//memset(explodedBallon, -1, sizeof(int) * 30);



		//SetEvent(updateEvent);
	}
#pragma endregion

#pragma region 업데이트 내용 클라이언트에 공유


#pragma endregion
	return NULL;
}

int main(int argc, char* argv[])
{
	int retval;

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// 소켓 생성
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET) err_quit("socket()");

	// bind()
	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(listen_sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("bind()");

	// listen()
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) err_quit("listen()");

	InitializeCriticalSection(&cs);
	// 데이터 통신에 사용할 변수
	SOCKET client_sock;
	struct sockaddr_in clientaddr;
	int addrlen;
	HANDLE hThread;

	for (int i = 0; i < 4; i++) {
		recvEvent[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
		event_data[i].ID = i;
		update_data[i].ID = i;
	}
	updateEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	
	Map_Init();
	while (1) {
		// accept()
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (struct sockaddr*)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET) {
			err_display("accept()");
			break;
		}

		// 접속한 클라이언트 정보 출력
		char addr[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));
		printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n",
			addr, ntohs(clientaddr.sin_port));

		// 스레드 생성
		hThread = CreateThread(NULL, 0, RecvThread,
			(LPVOID)client_sock, 0, NULL);
		if (hThread == NULL) { closesocket(client_sock); }
		else { CloseHandle(hThread); }

	}

	// 소켓 닫기
	closesocket(listen_sock);
	DeleteCriticalSection(&cs);
	// 윈속 종료
	WSACleanup();
	return 0;
}
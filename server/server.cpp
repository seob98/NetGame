#include "protocol.h"

#define SERVERPORT 9000
#define BUFSIZE    4096
#define MAX_PLAYER 4

int horzBlockCnt = 15;
int vertBlockCnt = 13;
int ballonID{};
int explodedBallon[30]{};

CLIENTINFO players[MAX_PLAYER];
SC_GAMEINFO player_data{};	// SC_GAMEINFO�� �迭���� ���� ������ ��ȯ
CS_EVENT event_data[4]{};
SC_PLAYERUPDATE update_data[4]{};

SC_BALLONBOMBEVENT update_ballondata{};

HANDLE recvEvent[4], updateEvent, uThread;
std::vector<CBlock> map;
std::vector<CObstacle> obstacles;
std::vector<CBallon> ballons{};
std::vector<CItem> items{};
std::vector<CWaterStream> waterstreams{};


CRITICAL_SECTION cs;
int cur_player = 0;

DWORD WINAPI UpdateThread(LPVOID arg);

void Map_Init()
{
	int itemtype;
	srand((unsigned int)time(NULL));
	// �� �ٴ� ��ġ ( ������ ����� ���� )
	for (int i = 0; i < 13; ++i)
	{
		for (int j = 0; j < 15; ++j)
		{
			map.emplace_back(POINT{ 40 + j * 40, 60 + i * 40 }, 20, i * 15 + j);
		}
	}

	// player_data�� itemtype�� blocktype�� ���� ä��� �Լ�

	int map_range = INDEX_MAPEND - INDEX_MAPSTART;
	for (int i = 0; i < map_range; i++) 
	{
		if (rand() % 10 <= 7)	// 70% Ȯ���� ��, 30%Ȯ���� ����
		{
			if (rand() % 10 <= 3)
			{
				player_data.Blockinfo[i].blocktype = 1;		//��
				obstacles.emplace_back(map[i + 30].GetPos(), map[i + 30].GetIndex(), map, true, -1);
			}
			else
			{
				player_data.Blockinfo[i].itemtype = rand() % 5 - 1;	// ���������� ������Ÿ�� ���� ����
				itemtype = player_data.Blockinfo[i].itemtype;
				player_data.Blockinfo[i].blocktype = 0;		//������
				obstacles.emplace_back(map[i + 30].GetPos(), map[i + 30].GetIndex(), map, false, itemtype);
			}
		}
		else
			player_data.Blockinfo[i].blocktype = -1;
	}

	//for (int i = 0; i < MAX_ITEM_CNT; i++)
	//{
	//	player_data.itemType[i].type = rand() % 7;
	//	player_data.itemType[i].pos = rand() % (map_range)+INDEX_MAPSTART;
	//}

}


unsigned short Player_Create(SOCKET sock)
{
	// ���� ���ӽ� �÷��̾� ���� ��� ����
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
	players[cur_player].player.SetMoving(false);
	players[cur_player].player.clientNum = cur_player;
	players[cur_player].player.SetState(p_state);
	cur_player++;

	if (cur_player == 4) {
		uThread = CreateThread(NULL, 0, UpdateThread,
			NULL, 0, NULL);
		if (uThread == NULL) {}
		else { CloseHandle(uThread); }
	}

	return players[cur_player - 1].ID;

}

void PlayerStateUpdate();

// Ŭ���̾�Ʈ�� ������ ���
DWORD WINAPI RecvThread(LPVOID arg)
{
	int retval{};
	SOCKET client_sock = (SOCKET)arg;
	struct sockaddr_in clientaddr;
	char addr[INET_ADDRSTRLEN];
	int addrlen{};
	int len{};
	char buf[BUFSIZE];

	// Ŭ���̾�Ʈ ���� ���
	addrlen = sizeof(clientaddr);
	getpeername(client_sock, (struct sockaddr*)&clientaddr, &addrlen);
	inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));

	int myID = Player_Create(client_sock);
	if (myID == -1)
		printf("���� ����\n");

	// Ŭ���̾�Ʈ �ʱ� ���� �����ֱ�
	player_data.ID = myID;
	player_data.gameStart = 0;
	player_data.currentPlayerCnt = cur_player;
	if (player_data.ID % 2 == 0)
		player_data.teamId = BAZZI;
	else
		player_data.teamId = DAO;

	retval = send(client_sock, (char*)&player_data, sizeof(SC_GAMEINFO), 0);
	printf("\n�ʱ� ���� ���� ����\n");

	if (retval == SOCKET_ERROR) {
		err_display("send()");
		return NULL;
	}

	if (player_data.ID > 0) {
		for (int i = 0; i < player_data.ID; i++) {
			retval = send(players[i].sock, (char*)&player_data, sizeof(SC_GAMEINFO), 0);
			printf("\n���� �������� �� ���� ���� ����\n");
		}
	}

	while (1) {
		//if (players[myID].player.GetState() >= 8)
		//	break;
		retval = recv(players[myID].sock, buf, sizeof(CS_EVENT), MSG_WAITALL);
		//if (retval == SOCKET_ERROR) {
		//	err_display("recv()");
		//	break;
		//}
		CS_EVENT* event = (CS_EVENT*)buf;
		EnterCriticalSection(&cs);
		

		event_data[myID].Index = event->Index;
		event_data[myID].moving = event->moving;
		event_data[myID].State = event->State;
		event_data[myID].setBallon = event->setBallon;
		event_data[myID].Dir = event->Dir;
		event_data[myID].usedNeedle = event->usedNeedle;
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
			players[i].player.Move(map, event_data[i].Dir);
			//printf("player[%d] speed : %d\n", i, players[i].player.GetSpeed());
			//players[i].player.MoveTrapped(map, event_data[i].State);
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

void PlayerCollisionCheck()
{
	std::vector<CPlayer*> ptPlayers;
	for (auto& p : players)
		ptPlayers.emplace_back(&(p.player));

	// ��ǳ�� �浹 ó��
	for (auto& ballon : ballons)
	{
		ballon.CheckPlayerOut(ptPlayers);
		ballon.CheckCollision(ptPlayers);
	}
	// ������ �浹 ó��
	for (auto& item : items)
	{
		item.CheckCollisionPlayers(ptPlayers);
		item.CheckCollisionWaterStreams(waterstreams);
	}
	// ���� ��ȭ ������Ʈ
	int speed, length, maxCnt;
	bool needle;
	for (int i = 0; i < 4; i++)
	{
		// �����ۿ� ���� �÷��̾� ���� ��ȭ
		speed = ptPlayers[i]->GetSpeed();
		length = ptPlayers[i]->GetBallonLength();
		maxCnt = ptPlayers[i]->GetBallonMaxCnt();
		needle = ptPlayers[i]->GetNeedle();
		players[i].player.setStat(speed, length, maxCnt, needle);
		// ��ǳ���� �÷��̾ �浹 ������ ���� ��ȭ
		players[i].player.SetState(ptPlayers[i]->Get_State());
		printf("player[%d] state = %d\n", players[i].player.clientNum, players[i].player.Get_State());
	}
}

void PlayerUpdateFrameOnce()	// 1ȸ�� ����Ǵ� �ִϸ��̼� (��ǳ�� ���� -> ����/��Ȱ���->IDLE)
{
	for (int i = 0; i < MAX_PLAYER; ++i)
	{
		players[i].player.Update_Frame_Once();
	}
}

void PlayerStateCheck()	// 1ȸ�� ����Ǵ� �ִϸ��̼� (��ǳ�� ���� -> ����/��Ȱ���->IDLE)
{
	for (int i = 0; i < MAX_PLAYER; ++i)
	{
		players[i].player.STATE_CHECK();
	}
}

void PlayerWaterstreamCollisionCheck()
{
	for (int i = 0; i < MAX_PLAYER; ++i)
	{
		players[i].player.CheckCollisionWaterStreams(waterstreams);
	}
}

void BallonUpdate()
{
	//memset(explodedBallon, -1, sizeof(int) * 30);
	memset(update_ballondata.explodedBomb, -1, sizeof(int) * 20);

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

			ballons.erase(ballons.begin() + i);
		}
		else
			++i;
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
			}

		}
	}
}

void WaterStreamUpdate()
{
	//���ٱ� ������Ʈ
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
	//��ֹ� ó�� (�����̳� ����)
	for (int i = 0; i < obstacles.size(); ++i)
	{
		obstacles[i].CheckExplosionRange(map);
		obstacles[i].makeItem(items);
	}
	for (int i = 0; i < obstacles.size();)
	{
		if (obstacles[i].GetDead())
			obstacles.erase(obstacles.begin() + i);
		else
			++i;
	}
}

void PlayerStateUpdate()
{
	for (int i = 0; i < MAX_PLAYER; i++)
	{
		if (event_data[i].State == -1)
			while (event_data[i].State <= -1);
		if (players[i].player.Get_State() != CPlayer::DIE && players[i].player.Get_State() != CPlayer::DEAD && players[i].player.Get_State() != CPlayer::TRAPPED)
			players[i].player.SetState((CPlayer::STATE)event_data[i].State);
		//printf("player[%d] State = %d\n", i, players[i].player.GetState());
	}
}

DWORD WINAPI UpdateThread(LPVOID arg)
{
	int retval;
#pragma region ������ü ������Ʈ
	while (1) 
	{
		//PlayerStateUpdate();
		WaitForMultipleObjects(cur_player, recvEvent, TRUE, 15);
		//WaitForMultipleObjects(4, recvEvent, FALSE, INFINITE);
		BallonUpdate();
		WaterStreamUpdate();
		ObstacleUpdate();
		PlaceBallon();

		PlayerMove();
		PlayerCollisionCheck();
		PlayerStateCheck();
		PlayerStateUpdate();
		PlayerUpdateFrameOnce();
		PlayerMapCollisionCheck();
		PlayerWaterstreamCollisionCheck();
		//SetEvent(updateEvent);


		// ������Ʈ ������
		for (int i = 0; i < 4; i++) {
			
			update_data[i].moving = players[i].player.isMoving();
			update_data[i].playerDir = players[i].player.GetDir();
			update_data[i].state = players[i].player.Get_State();
			update_data[i].pt = players[i].player.GetPos();
			update_data[i].setBallon = players[i].player.spaceButton();

		}

		for (auto& clients : players)
		{
			retval = send(clients.sock, (char*)&update_data, sizeof(SC_PLAYERUPDATE) * 4, 0);
			//if (retval == SOCKET_ERROR) {
			//	err_display("send()");
			//	break;
			//}


			retval = send(clients.sock, (char*)&update_ballondata, sizeof(SC_BALLONBOMBEVENT), 0);
			//if (retval == SOCKET_ERROR) {
			//	err_display("send()");
			//	break;
			//}
		}
	}
#pragma endregion

#pragma region ������Ʈ ���� Ŭ���̾�Ʈ�� ����


#pragma endregion
	return NULL;
}

int main(int argc, char* argv[])
{
	int retval;

	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// ���� ����
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
	// ������ ��ſ� ����� ����
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

		// ������ Ŭ���̾�Ʈ ���� ���
		char addr[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));
		printf("\n[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
			addr, ntohs(clientaddr.sin_port));

		// ������ ����
		hThread = CreateThread(NULL, 0, RecvThread,
			(LPVOID)client_sock, 0, NULL);
		if (hThread == NULL) { closesocket(client_sock); }
		else { CloseHandle(hThread); }

	}

	// ���� �ݱ�
	closesocket(listen_sock);
	DeleteCriticalSection(&cs);
	// ���� ����
	WSACleanup();
	return 0;
}
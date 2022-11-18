#include "protocol.h"

#define SERVERPORT 9000
#define BUFSIZE    4096
#define MAX_PLAYER 4

CLIENTINFO players[MAX_PLAYER];
SC_GAMEINFO player_data{};	// SC_GAMEINFO를 배열에서 단일 변수로 변환

std::vector<CBlock> map;

int cur_player = 0;

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
				player_data.blockType[i] = 1;		//돌
			else
				player_data.blockType[i] = 0;		//나무블럭
		}
		else
			player_data.blockType[i] = -1;
	}

	for (int i = 0; i < MAX_ITEM_CNT; i++)
	{
		player_data.itemType[i].type = rand() % 7;
		player_data.itemType[i].pos = rand() % (map_range) + INDEX_MAPSTART;
	}


	// 테스트 용도 : 첫번째 블럭을 -1 (설치 안함)
	player_data.blockType[0] = -1;

	// 테스트 전용 코드
	for (int a : player_data.blockType)
		printf("[%d]\t", a);
	printf("\n");
	for (auto a : player_data.itemType)
		printf("[%d,%d]\t", a.pos, a.type);
	// 테스트 전용 코드



}


unsigned short Player_Create(SOCKET sock)
{
	// 게임 접속시 플레이어 상태 대기 상태
	CPlayer::STATE p_state = CPlayer::STATE::IDLE;
	POINT pos;
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

	printf("player id: %d, x: %d, y: %d\n", players[cur_player].ID,
		pos.x, pos.y);

	return players[cur_player - 1].ID;
	
}


// 클라이언트와 데이터 통신
DWORD WINAPI RecvThread(LPVOID arg)
{
	int retval;
	SOCKET client_sock = (SOCKET)arg;
	struct sockaddr_in clientaddr;
	char addr[INET_ADDRSTRLEN];
	int addrlen;
	int len;
	char buf[BUFSIZE + 1];

	// 클라이언트 정보 얻기
	addrlen = sizeof(clientaddr);
	getpeername(client_sock, (struct sockaddr*)&clientaddr, &addrlen);
	inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));
	
	retval = Player_Create(client_sock);
	if (retval == -1)
		printf("생성 실패\n");

	// 클라이언트 초기 정보 보내주기
	player_data.ID = retval;
	player_data.gameStart = 0;
	player_data.currentPlayerCnt = retval;
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

	//while (1) {
	//	retval = recv(players[p_data.ID].sock, buf, sizeof(CS_EVENT), MSG_WAITALL);
	//	if (retval == SOCKET_ERROR) {
	//		err_display("recv()");
	//		break;
	//	}
	//	switch (KEY_EVENT) {
	//	case PRESS_LEFT:
	//		SC_PLAYERUPDATE p_update;
	//		p_update.ID = players[p_data.ID].ID;
	//		p_update.pt.x = 
	//		break;
	//	case PRESS_RIGHT:
	//		break;
	//	case PRESS_UP:
	//		break;
	//	case PRESS_DOWN:
	//		break;
	//	case PRESS_SPACE:
	//		break;
	//	case PRESS_ITEM:
	//		break;
	//	default:
	//		break;
	//	}
	//}
	return 0;
}

DWORD WINAPI UpdateThread(LPVOID arg)
{
	int retval;
	SOCKET client_sock = (SOCKET)arg;
	struct sockaddr_in clientaddr;
	char addr[INET_ADDRSTRLEN];
	int addrlen;
	int len;
	char buf[BUFSIZE + 1];

	// 클라이언트 정보 얻기
	addrlen = sizeof(clientaddr);
	getpeername(client_sock, (struct sockaddr*)&clientaddr, &addrlen);
	inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));

#pragma region 서버객체 업데이트
	while (1) {

	}
#pragma endregion

#pragma region 업데이트 내용 클라이언트에 공유


#pragma endregion

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

	// 데이터 통신에 사용할 변수
	SOCKET client_sock;
	struct sockaddr_in clientaddr;
	int addrlen;
	HANDLE hThread;

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

	// 윈속 종료
	WSACleanup();
	return 0;
}
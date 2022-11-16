#include "protocol.h"

#define SERVERPORT 9000
#define BUFSIZE    4096
#define MAX_PLAYER 4

CLIENTINFO players[MAX_PLAYER];
SC_GAMEINFO player_data{};	// SC_GAMEINFO�� �迭���� ���� ������ ��ȯ

int cur_player = 0;

void Map_Init()
{
	// player_data�� itemtype�� blocktype�� ���� ä��� �Լ�

	int map_range = INDEX_MAPEND - INDEX_MAPSTART;
	for (int i = 0; i < map_range; i++) {
		if (rand() % 10 <= 7)	// 70% Ȯ���� ��, 30%Ȯ���� ����
			player_data.blockType[i] = 1;
		else
			player_data.blockType[i] = 0;
	}

	for (int i = 0; i < MAX_ITEM_CNT; i++)
	{
		player_data.itemType[i].type = rand() % 7;
		player_data.itemType[i].pos = rand() % (map_range) + INDEX_MAPSTART;
	}


	// �׽�Ʈ ���� �ڵ�
	for (int a : player_data.blockType)
		printf("[%d]\t", a);
	printf("\n");
	for (auto a : player_data.itemType)
		printf("[%d,%d]\t", a.pos, a.type);
	// �׽�Ʈ ���� �ڵ�
}

int Player_Create(LPVOID arg)
{
	SOCKET s_find = (SOCKET)arg;
	unsigned short index = -1;

	// player������ ã�Ƽ� ��ġ ����
	for (int i = 0; i < MAX_PLAYER; i++) {
		if (s_find == players[i].sock) {
			index = players[i].ID;
		}
		else {
			printf("������ ��ã�ҽ��ϴ�.\n");
			return -1;
		}
	}

	// �÷��̾� ��ġ ����
	if (index == 0) {
		players[index].player.p_pos.x = 0;
		players[index].player.p_pos.y = 0;
		printf("�÷��̾� id: %d, �÷��̾� pos_x : %d, pos_y : %d\n", index, 
			players[index].player.p_pos.x, index, players[index].player.p_pos.x);
	}
	if (index == 1) {
		players[index].player.p_pos.x = 100;
		players[index].player.p_pos.y = 150;
		printf("�÷��̾� id: %d, �÷��̾� pos_x : %d, pos_y : %d\n", index, 
			players[index].player.p_pos.x, index, players[index].player.p_pos.x);
	}
	if (index == 2) {
		players[index].player.p_pos.x = 500;
		players[index].player.p_pos.y = 550;
		printf("�÷��̾� id: %d, �÷��̾� pos_x : %d, pos_y : %d\n", index, 
			players[index].player.p_pos.x, index, players[index].player.p_pos.x);
	}
	if (index == 3) {
		players[index].player.p_pos.x = 600;
		players[index].player.p_pos.y = 650;
		printf("�÷��̾� id: %d, �÷��̾� pos_x : %d, pos_y : %d\n", index, 
			players[index].player.p_pos.x, index, players[index].player.p_pos.x);
	}
	return 1;
}



// Ŭ���̾�Ʈ�� ������ ���
DWORD WINAPI RecvThread(LPVOID arg)
{
	int retval;
	SOCKET client_sock = (SOCKET)arg;
	struct sockaddr_in clientaddr;
	char addr[INET_ADDRSTRLEN];
	int addrlen;
	int len;
	char buf[BUFSIZE + 1];

	// Ŭ���̾�Ʈ ���� ���
	addrlen = sizeof(clientaddr);
	getpeername(client_sock, (struct sockaddr*)&clientaddr, &addrlen);
	inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));
	
	retval = Player_Create((LPVOID)client_sock);
	if (retval == -1)
		printf("���� ����\n");
	else if (retval == 1)
		printf("���� ����\n");

	// Ŭ���̾�Ʈ �ʱ� ���� �����ֱ�
	SC_GAMEINFO p_data{};
	p_data.ID = cur_player;
	cur_player++;
	p_data.gameStart = 0;

	retval = send(players[cur_player].sock, (char*)&p_data, sizeof(SC_GAMEINFO), 0);
	if (retval == SOCKET_ERROR) {
		err_display("send()");
		return NULL;
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

	// Ŭ���̾�Ʈ ���� ���
	addrlen = sizeof(clientaddr);
	getpeername(client_sock, (struct sockaddr*)&clientaddr, &addrlen);
	inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));

#pragma region ������ü ������Ʈ
	while (1) {

	}
#pragma endregion

#pragma region ������Ʈ ���� Ŭ���̾�Ʈ�� ����


#pragma endregion

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

	// ������ ��ſ� ����� ����
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

	// ���� ����
	WSACleanup();
	return 0;
}
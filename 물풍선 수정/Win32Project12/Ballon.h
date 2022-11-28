#pragma once

class CObstacle;
class CWaterStream;


class CBallon
{
public:
	CBallon(POINT Pos, int Size, int Number, int _explosionLength, bool _player0, int _summonerClientNum, CPlayer* _summoner);

private:
	int id;
	POINT pos{};
	int size{};
	RECT rt{};
	int index{};			//���μ����� 1,2,3.. ��° ��.

	CPlayer* summoner{};	//player0

	int summonerClientNum{};
	
	int listOfIntersected[MAX_PLAYER_CNT];


	bool player0{};			//������ �����Ϸ���

	FRAME frame{};

	int drawID{ };

	float explosionTime{ 13 };
	float explosionSpeed{0.1f};
	bool exploded;
	int explosionLength{ 5 };

private:
#pragma region ����ź ���� ����
	int goLeft{};
	int goRight{};
	int goUp{};
	int goDown{};

	int LeftLocation{};
	int RightLocation{};
	int UpLocation{};
	int DownLocation{};

	int LeftDelayTime{};
	int UpDelayTime{};
	int RightDelayTime{};
	int DownDelayTime{};

	int streamDelayTime = 2.f;
#pragma endregion
public:
	int GetIndex() { return index; }
	bool GetPlayer0() { return player0; }
public:
	void Draw(HDC hdc);
	void CheckPlayerOut(std::vector<CPlayer>& players);	//��ź ��ġ�� �ۿ� ������ �ִ� �� Ȯ��
	void CheckCollision(std::vector<CPlayer>& players);

	void UpdateFrame();
	bool getExplode() { return exploded; }

public:
	void Update(std::vector<CBallon>& ballons, std::vector<CWaterStream>& waterStreams, std::vector<CBlock>& blocks, std::vector<CObstacle>& obstacles, int horzBlockCnt, int vertBlockCnt);
	void BallonTimeCheck();	// ��ź �ð� ����. 0�̶�� ���д�.
	void CheckDeadEnd(int dir, int& temp, int& suggestedIndex, std::vector<CObstacle>& obstacles, int obstacleTimeSet, int horzBlockCnt, int vertBlockCnt);
															// ����ź�� �� �� �� �ִ� �������� �Ǵ�. �μ��� �� �ִ� ��ֹ��� ��� �����ȿ� �ִٰ� üũ�� + ���� �ð� ����.
															// ���ο��� ���ٱ� ���� �� �� ���� �ð� üũ -> �� ���� �ð� �ٿ� -> �� �����ð��� �� ��� ����.
	void CheckChainExplosion(std::vector<CBallon>& ballons, std::vector<CWaterStream>& waterStreams, std::vector<CBlock>& blocks, std::vector<CObstacle>& obstacles, int horzBlockCnt, int vertBlockCnt);
															// ���Լ��� ���� üũ�� waterstream ����
};


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
	int index{};			//가로순으로 1,2,3.. 번째 블럭.

	CPlayer* summoner{};	//player0

	int summonerClientNum{};
	
	int listOfIntersected[MAX_PLAYER_CNT];


	bool player0{};			//누군지 구분하려고

	FRAME frame{};

	int drawID{ };

	float explosionTime{ 13 };
	float explosionSpeed{0.1f};
	bool exploded;
	int explosionLength{ 5 };

private:
#pragma region 물폭탄 관련 변수
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
	void CheckPlayerOut(std::vector<CPlayer>& players);	//폭탄 설치후 밖에 나온적 있는 지 확인
	void CheckCollision(std::vector<CPlayer>& players);

	void UpdateFrame();
	bool getExplode() { return exploded; }

public:
	void Update(std::vector<CBallon>& ballons, std::vector<CWaterStream>& waterStreams, std::vector<CBlock>& blocks, std::vector<CObstacle>& obstacles, int horzBlockCnt, int vertBlockCnt);
	void BallonTimeCheck();	// 폭탄 시간 빼기. 0이라면 냅둔다.
	void CheckDeadEnd(int dir, int& temp, int& suggestedIndex, std::vector<CObstacle>& obstacles, int obstacleTimeSet, int horzBlockCnt, int vertBlockCnt);
															// 물폭탄이 더 갈 수 있는 구간인지 판단. 부서질 수 있는 장애물의 경우 범위안에 있다고 체크함 + 죽을 시간 설정.
															// 메인에서 물줄기 생성 후 블럭 삭제 시간 체크 -> 블럭 삭제 시간 줄여 -> 블럭 삭제시간이 될 경우 삭제.
	void CheckChainExplosion(std::vector<CBallon>& ballons, std::vector<CWaterStream>& waterStreams, std::vector<CBlock>& blocks, std::vector<CObstacle>& obstacles, int horzBlockCnt, int vertBlockCnt);
															// 윗함수로 영역 체크후 waterstream 생성
};


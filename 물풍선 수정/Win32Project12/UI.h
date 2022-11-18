#pragma once
class CUI
{
public:
	CUI();

private:
	bool dead{};
public:
	void DrawStartUI(HDC hdc, bool pressStartButton, int currentPlayerCnt);
	void DrawLoadingUI(HDC hdc, bool pressStartButton, int currentPlayerCnt);
	void DrawMainframeUI(HDC hdc);
	void Update();
public:
	bool pressGameStart{};
	bool GameStart{};
public:
	bool GetDead() { return dead; }

public:
};


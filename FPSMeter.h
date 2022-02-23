#pragma once
#include <windows.h>
#include <string>
class FPSMeter
{
public:
	FPSMeter(HWND hwnd, const std::wstring& title);
	void TickStart();
	void TickEnd();
private:
	int tickNum = 0;
	double tickTotalInterval = 0, prevTime = 0, lastFrameTime = 0;
	HWND hwnd;
	std::wstring title;
	void ShowFPS(int fps, double avgInterval);
};


#include <sys/timeb.h>
#include <istream>
#include <sstream>
#include "FPSMeter.h"

FPSMeter::FPSMeter(HWND hwnd, const std::wstring& title)
{
	timeb tb;
	ftime(&tb);
	lastFrameTime = tb.time + 1.0 * tb.millitm / 1000;
	this->hwnd = hwnd;
	this->title = title;
}

void FPSMeter::TickStart()
{
	timeb tb;
	ftime(&tb);
	prevTime = tb.time + 1.0 * tb.millitm / 1000;
}

void FPSMeter::TickEnd()
{

	timeb tb;
	ftime(&tb);
	double curTime = tb.time + 1.0 * tb.millitm / 1000;
	tickNum++;
	tickTotalInterval += curTime - prevTime;
	if (curTime - lastFrameTime >= 1)
	{
		ShowFPS(tickNum, tickTotalInterval / tickNum * 1000);
		tickNum = 0;
		tickTotalInterval = 0;
		lastFrameTime = curTime;
	}
}
void FPSMeter::ShowFPS(int fps, double avgInterval)
{
	std::wstringstream ss;
	ss.precision(2);
	ss.setf(std::ios::fixed);
	ss << title << L"       帧数：" << fps << L" 平均每帧：" << avgInterval << L"ms";
	auto ans = SetWindowText(hwnd, ss.str().c_str());
}
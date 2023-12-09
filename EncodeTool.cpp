#include "encodeTool.h"
const wchar_t* GetWC(const char* c)
{
	const size_t cSize = strlen(c) + 1;
	wchar_t* wc = new wchar_t[cSize];
	mbstowcs_s(NULL, wc, cSize, c, cSize);
	return wc;

}
const char* GetC(const wchar_t* wc) {
	const size_t cSize = wcslen(wc) + 1;
	char* c = new char[cSize];
	wcstombs_s(NULL, c, cSize, wc, cSize);
	return c;

}
std::wstring s2ws(const std::string& s)
{
	std::wstring wc(s.length(), L' ');
	mbstowcs_s(NULL, &wc[0], s.length() + 1, s.c_str(), s.length());
	return wc;
}
std::string ws2s(const std::wstring& ws)
{
	std::string s(ws.length(), ' ');
	wcstombs_s(NULL, &s[0], ws.length() + 1, ws.c_str(), ws.length());
	return s;
}
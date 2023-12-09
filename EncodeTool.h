#pragma once
#include <string>

const wchar_t* GetWC(const char* c);
const char* GetC(const wchar_t* wc);
std::wstring s2ws(const std::string& s);
std::string ws2s(const std::wstring& ws);
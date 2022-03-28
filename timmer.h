#pragma once
#if defined(__linux__) || defined(__unix__)
#define sleep sleep_unix
#elif defined(_WIN32)
#define sleep sleep_windows
#pragma comment(lib,"Winmm.lib")
#pragma comment(lib,"ws2_32.lib")  
#endif // 

void sleep_unix(unsigned const int millisecond);
void sleep_windows(unsigned const int millisecond);
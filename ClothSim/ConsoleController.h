/*
   ______                       __        ________       __
  / ____/___  ____  _________  / /__     / ____/ /______/ /
 / /   / __ \/ __ \/ ___/ __ \/ / _ \   / /   / __/ ___/ /
/ /___/ /_/ / / / (__  ) /_/ / /  __/  / /___/ /_/ /  / /
\____/\____/_/ /_/____/\____/_/\___/   \____/\__/_/  /_/

Henry Oliver

Verison 4.2

*/

#pragma once
#include <windows.h> 
#include <iostream>
#include <chrono>
#include <string>
#include <vector>  
#include <stdlib.h>  
#include <time.h>  
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <locale>
#include <codecvt>
#include <string>

using namespace std;

/*

COLOR CODES:

RANGE: 0-254

0 - BLACK
1 - DARK BLUE
2 - DARK GREEN
3 - DARK AQUA
4 - DARK RED
5 - DARK PINK
6 - DARK YELLOW
7 - LIGHT GREY
8 - GREY
9 - BLUE
10 - GREEN
11 - AQUA
12 - RED
13 - PINK
14 - YELLOW
15 - WHITE

0-15 BLACK BACK
16-31 DARK BLUE BACK
32-47 DARK GREEN BACK
48-63 DARK AQUA BACK
64-79 DARK RED BACK
80-95 DARK PINK BACK
96-111 DARK YELLOW BACK
112-127 LIGHT GREY BACK
128-143 GREY BACK
144-159 BLUE BACK
160-175 GREEN BACK
176-191 AQUA BACK
192-207 RED BACK
208-223 PINK BACK
224-239 YELLOW BACK
240-254 WHITE BACK

*/

enum LOGCOLOR
{
	DEFAULT = 15,
	LOGINFO = 10,
	LOGWARN = 6,
	LOGFATAL = 12,
};

inline bool fullLogColor = false;

inline void Console_gotoXY(int x, int y) {
	COORD coord;
	coord.X = x;
	coord.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

inline void Console_Resize(int x, int y) { //Resize Console Window
	RECT m_rect;
	HWND m_console = GetConsoleWindow();
	GetWindowRect(m_console, &m_rect); //stores the console's current dimensions
	MoveWindow(m_console, m_rect.left, m_rect.top, x, y, TRUE);
}

inline void Console_Clear() { //Clear console window
	system("cls");
	Console_gotoXY(0, 0);
}


inline void Console_FontSize(int x, int y) {
	HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
	PCONSOLE_FONT_INFOEX lpConsoleCurrentFontEx = new CONSOLE_FONT_INFOEX();
	lpConsoleCurrentFontEx->cbSize = sizeof(CONSOLE_FONT_INFOEX);
	GetCurrentConsoleFontEx(out, 0, lpConsoleCurrentFontEx);
	lpConsoleCurrentFontEx->dwFontSize.X = x;
	lpConsoleCurrentFontEx->dwFontSize.Y = y;
	SetCurrentConsoleFontEx(out, 0, lpConsoleCurrentFontEx);
}

inline void Console_ColoredTEXT(wstring m_word, int m_color) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), m_color);
	wcout << m_word;
}

inline void Console_ColoredTEXTChar(char m_word, int m_color) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), m_color);
	cout << m_word;
}

inline void Console_OutputLog(std::wstring log, int type) {
	Console_ColoredTEXT(L"[", DEFAULT);
	switch (type)
	{
	case LOGINFO: {
		Console_ColoredTEXT(L"INFO", LOGINFO);
		break;
	}
	case LOGWARN: {
		Console_ColoredTEXT(L"WARN", LOGWARN);
		break;
	}
	case LOGFATAL: {
		Console_ColoredTEXT(L"FATAL", LOGFATAL);
		break;
	}
	default: {
		Console_ColoredTEXT(L"UNKN", DEFAULT);
		break;
	}
	}
	Console_ColoredTEXT(L"] ", DEFAULT);
	log = log + L"\n";
	if (fullLogColor) {
		Console_ColoredTEXT(log, type);
	}
	else {
		Console_ColoredTEXT(log, DEFAULT);
	}
}


inline void Banner() {
	int color = rand() % 15 + 1;
	Console_ColoredTEXT(L"===============================================================\n", color);
	Console_ColoredTEXT(L"    _______                      __        ________       __ \n", color);
	Console_ColoredTEXT(L"   / ____ /___ ____  _________  / /__     / ____/ /______/ / \n", color);
	Console_ColoredTEXT(L"  / /   / __ \\/ __ \\/ ___/ __ \\/ / _ \\   / /   / __/ ___/ / \n", color);
	Console_ColoredTEXT(L" / /___/ /_/ / / / (__  ) /_/ / /  __/  / /___/ /_/ /  / /  \n", color);
	Console_ColoredTEXT(L" \\____/\\____/_/ /_/____/\\____/_/\\___/   \\____/\\__/_/  /_/   \n", color);
	Console_ColoredTEXT(L" ~ Henry Oliver                                    v 4.2\n", color); \
		Console_ColoredTEXT(L"===============================================================\n", color);
	Console_ColoredTEXT(L"\n", 15);

}

inline void Console_Initalize() {
	_setmode(_fileno(stdout), _O_U16TEXT);
	srand((unsigned int)time(NULL));
	Banner();
}

inline wstring to_wstring(string str)
{
	int wchars_num = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
	wchar_t* wstr = new wchar_t[wchars_num];
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, wstr, wchars_num);
	return wstr;
}

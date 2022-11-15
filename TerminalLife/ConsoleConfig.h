#pragma once
#include "pch.h"

class ConsoleConfig
{
private:
    DWORD _dwOriginalOutMode = 0;
    HANDLE _hOut = 0;
    CONSOLE_SCREEN_BUFFER_INFO _csbi = {};

public:
    ConsoleConfig();
    ~ConsoleConfig();

    void Clear() const
    {
        system("CLS");
    }

    int Width() const
    {
        return _csbi.dwSize.X;
    }

    int Height() const
    {
        return _csbi.dwSize.Y;
    }

    void DrawBegin();

    void SetPositionHome()
    {
        static COORD coordScreen = { 0, 0 };
        SetConsoleCursorPosition(_hOut, coordScreen);
    }

    void SetPositionBoard()
    {
        static COORD coordScreen = { 0, 5 };
        SetConsoleCursorPosition(_hOut, coordScreen);
    }

    void SetPosition(short x, short y)
    {
        static COORD coordScreen = { x, y };
        SetConsoleCursorPosition(_hOut, coordScreen);
    }
};

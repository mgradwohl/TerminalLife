#pragma once
extern class Board;

class HUD
{
public:
    static HUD& Get()
    {
        static HUD s_Instance;
        return s_Instance;
    }

    HUD() {}
    HUD(const HUD&) = delete;

    ~HUD() {}


#ifdef _DEBUG
    int _msSleep = 50;
    bool _fFate = true;
    bool _fScore = true;
    bool _fIncremental = false;
    bool _fOldAge = false;
#else
    int _msSleep = 0;
    bool _fFate = false;
    bool _fScore = true;
    bool _fIncremental = false;
    bool _fOldAge = false;
#endif

public:
    int Delay()
    {
        return _msSleep;
    }

    bool Fate()
    {
        return _fFate;
    }

    bool Score()
    {
        return _fScore;
    }

    bool Incremental()
    {
        return _fIncremental;
    }

    int OldAge()
    {
        if (_fOldAge)
            return 80;
        else
            return -1;
    }

    bool CheckKeyState();
    void PrintIntro();
    bool Update(Board& board);
    void HandleIncremental();
};

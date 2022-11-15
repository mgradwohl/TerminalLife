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

    HUD() = default;
    HUD(const HUD&) = delete;

    ~HUD() = default;


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

    int Delay() const
    {
        return _msSleep;
    }

    bool Fate() const
    {
        return _fFate;
    }

    bool Score() const
    {
        return _fScore;
    }

    bool Incremental() const
    {
        return _fIncremental;
    }

    int OldAge() const
    {
        if (_fOldAge)
            return 80;
        else
            return -1;
    }

    bool CheckKeyState();
    void PrintIntro() const;
    bool Update(const Board& board) const;
    void HandleIncremental() const;
};

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

    static int Delay()
    {
        return Get()._msSleep;
    }

    static bool Fate()
    {
        return Get()._fFate;
    }

    static bool Score()
    {
        return Get()._fScore;
    }

    static bool Incremental()
    {
        return Get()._fIncremental;
    }

    static int OldAge()
    {
        return Get().OldAgeImpl();
    }

    static bool CheckKeyState()
    {
        return Get().CheckKeyStateImpl();
    }

    static void PrintIntro()
    {
        Get().PrintIntroImpl();
    }
    
    static bool Update(const Board& board)
    {
        return Get().UpdateImpl(board);
    }

    static void HandleIncremental()
    {
        Get().HandleIncrementalImpl();
    }

private:
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

    int OldAgeImpl() const
    {
        if (_fOldAge)
            return 80;
        else
            return -1;
    }

    bool CheckKeyStateImpl();
    void PrintIntroImpl() const;
    bool UpdateImpl(const Board& board) const;
    void HandleIncrementalImpl() const;
};

#pragma once
class Cell
{
public:
    enum class State { Dead, Born, Live, Old, Dying };

private:
    State _state;
    int _age, _x, _y, _neighbors;
    inline static int numDead = 0;
    inline static int numLive = 0;
    inline static int numBorn = 0;
    inline static int numOld = 0;
    inline static int numDying = 0;
    inline static int OldAge = -1;

public:
    //Cell(const Cell&) = delete;
    ~Cell() = default;
    Cell const& operator=(Cell& cell) = delete;

    Cell() : _state(State::Dead), _age(0), _x(0), _y(0), _neighbors(0)
    {
    }

    //// the compiler will not let me delete the copy constructor, AND when I implement them
    //// they don't seem to be called
    //Cell(const Cell& c)
    //    : _state(c._state), _age(c._age), _x(c._x), _y(c._y), _neighbors(c._y)
    //{
    //    std::cout << "Cell const copy constructor" << std::endl;
    //    std::cin.get();
    //}

    static void SetOldAge(int age)
    {
        OldAge = age;
    }

    static int GetOldAge()
    {
        return OldAge;
    }

    static int GetDeadCount()
    {
        return numDead;
    }

    static int GetLiveCount()
    {
        return numLive;
    }

    static int GetBornCount()
    {
        return numBorn;
    }

    static int GetOldCount()
    {
        return numOld;
    }

    static int GetDyingCount()
    {
        return numDying;
    }

    static bool ResetCounts()
    {
        numDead = 0;
        numLive = 0;
        numBorn = 0;
        numDying = 0;
        numOld = 0;
        return true;
    }

    void SetXY(int x, int y)
    {
        _x = x;
        _y = y;
    }

    int X() const
    {
        return _x;
    }

    int Y() const
    {
        return _y;
    }

    int Neighbors() const
    {
        return _neighbors;
    }

    void SetNeighbors(int n)
    {
        _neighbors = n;
    }

    void SetAge(int age)
    {
        _age = age;
    }

    int Age() const
    {
        return _age;
    }

    void SetState(State state);

    State GetState() const
    {
        return _state;
    }

    bool IsAlive() const;

    bool IsAliveNotDying() const;

    bool IsDead() const;

    const char* GetStateString() const;

    const char8_t* GetEmojiStateString() const;

    void NextGeneration();

    void KillOldCell();
};

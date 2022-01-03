// TerminalLife.cpp
#include <windows.h>
#include <iostream>
#include <vector>
#include <functional>
#include <string>

class Cell
{
public:
    // in many implementations a new copy of the board is made
    // and the new board is by looking at the old board.
    // I do this with four states: Dead, Born, Live, Dying.
    // Born means "will be alive in the next generation"
    // Dying means "will die in the next generation"
    // an advantage is that the board never changes size after instantiation
    enum class State { Dead, Born, Live, Dying };

private:
    int _x, _y, _age, _neighbors;
    State _state;
    inline static int numDead = 0;
    inline static int numLive = 0;
    inline static int numBorn = 0;
    inline static int numDying = 0;

public:
    Cell()
        : _state(State::Dead), _age(0), _x(0), _y(0), _neighbors(0)
    {
        //std::wcout << "Cell constructor Cell()" << std::endl;
    }

    Cell(int x, int y) 
        : _state(State::Dead), _age(0), _x(x), _y(y), _neighbors(0)
    {
        //std::wcout << "Cell constructor Cell(x,y)" << std::endl;
    }

    Cell(State state)
        : _state(state), _age(0), _x(0), _y(0), _neighbors(0)
    {
        if (_state == Cell::State::Born) _age = 0;
        //std::wcout << "Cell constructor Cell(state)" << std::endl;
    }

    Cell(const Cell& c)
        : _state(c._state), _age(c._age), _x(c._x), _y(c._y), _neighbors(c._y)
    {
        //std::wcout << "Cell copy constructor" << std::endl;
    }

    ~Cell()
    {}

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

    int Age() const
    {
        return _age;
    }

    void SetState(State state)
    {
        _state = state;

        switch (state)
        {
            case Cell::State::Dead: numDead++;
                break;
            case Cell::State::Live: numLive++;
                break;
            case Cell::State::Born: numBorn++;
                break;
            case Cell::State::Dying: numDying++;
                break;
        }
    }

    State GetState() const
    {
        return _state;
    }

    bool IsAlive() const
    {
        if (_state == Cell::State::Live || _state == Cell::State::Dying)
        {
            return true;
        }
        return false;
    }

    bool IsAliveNotDying() const
    {
        if (_state == Cell::State::Live)
        {
            return true;
        }
        return false;
    }

    bool IsDead() const
    {
        if (_state == Cell::State::Dead || _state == Cell::State::Born)
        {
            return true;
        }
        return false;
    }

    const char* GetStateString() const
    {
        switch (_state)
        {
        case State::Dead: return " ";
            break;
        case State::Born: return "o";
            break;
        case State::Live: return "O";
            break;
        case State::Dying: return ".";
            break;
        default:
            return "?";
        }
    }

    const wchar_t* GetWideStateString() const
    {
        static const std::wstring strDead(L" ");
        static const std::wstring strLive(L"\x1b[mO");
        static const std::wstring strBorn(L"\x1b[32mo");
        static const std::wstring strDying(L"\x1b[31mo");
        static const std::wstring strUnkown(L"\x1b[31m?");

        //static const std::wstring strDead(L"💩");
        //static const std::wstring strLive(L"😁");
        //static const std::wstring strBorn(L"👼");
        //static const std::wstring strDying(L"💔");
        //static const std::wstring strUnkown(L"⁉️");

        switch (_state)
        {
        case State::Dead: return strDead.c_str();
            break;
        case State::Live: return strLive.c_str();
            break;
        case State::Born: return strBorn.c_str();
            break;
        case State::Dying: return strDying.c_str();
            break;
        default:
            return strUnkown.c_str();
        }
    }

    const auto GetEmojiStateString() const
    {
        //static const std::wstring strDead(L" ");
        //static const std::wstring strLive(L"\x1b[mO");
        //static const std::wstring strBorn(L"\x1b[32mo");
        //static const std::wstring strDying(L"\x1b[31mo");
        //static const std::wstring strUnkown(L"\x1b[31m?");

        static auto cDead    = u8"🖤\0";
        static auto cLive    = u8"😁\0";
        static auto cBorn    = u8"👼\0";
        static auto cDying   = u8"🤢\0";
        static auto cUnknown = u8"⁉️\0";

        switch (_state)
        {
            case State::Dead: return cDead;
                break;
            case State::Live: return cLive;
                break;
            case State::Born: return cBorn;
                break;
            case State::Dying: return cDying;
                break;
            default:
                return cUnknown;
        }
    }

    void NextGeneration()
    {
        if (_state == Cell::State::Dead)
        {
            // no birthday for you
            return;
        }

        if (_state == Cell::State::Born)
        {
            SetState(Cell::State::Live);
        }

        if (_state == Cell::State::Dying)
        {
            SetState(Cell::State::Dead);
        }

        _age++;
        // Set a positive int to kill old cells when they reach age==int
        KillOldCells(-1);
    }

    void KillOldCells(int age)
    {
        if (age == -1)
            return;

        if (_age >= age)
            SetState(Cell::State::Dead);
    }
};

// for visualization purposes (0,0) is the top left.
// as x increases move right
// as y increases move down
class Board
{
private:
    // if I allocated this on the heap, I could get the size right with resize
    std::vector<Cell> _board;
    int _width, _height;
    int _size;
    int _generation;
    int _x, _y;

public:
    Board(int width, int height)
        : _width(width), _height(height), _size(width* height), _generation(0), _x(0), _y(0)
    {
        //std::wcout << "Board constructor Board(w, h)" << std::endl;
        _board.resize(_size);
        for (int x = 0; x < _width; x++)
        {
            for (int y = 0; y < _height; y++)
            {
                // gotta be a better way?
                _board[x + (y * _width)].SetXY(x, y);
            }
        }
    }

    Board(const Board& b)
        : _width(b._width), _height(b._height), _size(b._size), _generation(b._generation), _x(b._x), _y(b._y)
    {
        //std::wcout << "Board copy constructor" << std::endl;
    }

    int Generation() const
    {
        return _generation;
    }

    int Width() const
    {
        return _width;
    }

    int Height() const
    {
        return _height;
    }

    void SetCell(int x, int y, Cell::State state)
    {
        // no bounds checking
        Cell& cell = _board[x + (y * _width)];
        cell.SetState(state);
    }

    Cell& GetCell(int x, int y)
    {
        // no bounds checking
        return _board[x + (y * _width)];
    }

    Cell& CurrentCell()
    {
        return _board[_x + (_y * _width)];
    }

    int CountLiveNeighbors(Cell& cell)
    {
        int x = cell.X();
        int y = cell.Y();

        // calculate offsets that wrap
        int xoleft =  (x == 0) ? _width - 1 : -1;
        int xoright = (x == (_width - 1)) ? -(_width - 1) : 1;
        int yoabove = (y == 0) ? _height - 1 : -1;
        int yobelow = (y == (_height - 1)) ? -(_height - 1) : 1;

        int count = 0;

        if (GetCell(x + xoleft, y + yobelow).IsAlive()) count++;
        if (GetCell(x, y + yobelow).IsAlive()) count++;
        if (GetCell(x + xoright, y + yobelow).IsAlive()) count++;

        if (GetCell(x + xoleft, y + yoabove).IsAlive()) count++;
        if (GetCell(x, y + yoabove).IsAlive()) count++;
        if (GetCell(x + xoright, y + yoabove).IsAlive()) count++;

        if (GetCell(x + xoleft, y).IsAlive()) count++;
        if (GetCell(x + xoright, y).IsAlive()) count++;

        cell.SetNeighbors(count);

        return count;
    }

    int CountLiveNotDyingNeighbors(Cell& cell)
    {
        int x = cell.X();
        int y = cell.Y();

        // calculate offsets that wrap
        int xoleft = (x == 0) ? _width - 1 : -1;
        int xoright = (x == (_width - 1)) ? -(_width - 1) : 1;
        int yoabove = (y == 0) ? _height - 1 : -1;
        int yobelow = (y == (_height - 1)) ? -(_height - 1) : 1;

        int count = 0;

        if (GetCell(x + xoleft, y + yobelow).IsAliveNotDying()) count++;
        if (GetCell(x, y + yobelow).IsAliveNotDying()) count++;
        if (GetCell(x + xoright, y + yobelow).IsAliveNotDying()) count++;

        if (GetCell(x + xoleft, y + yoabove).IsAliveNotDying()) count++;
        if (GetCell(x, y + yoabove).IsAliveNotDying()) count++;
        if (GetCell(x + xoright, y + yoabove).IsAliveNotDying()) count++;

        if (GetCell(x + xoleft, y).IsAliveNotDying()) count++;
        if (GetCell(x + xoright, y).IsAliveNotDying()) count++;

        cell.SetNeighbors(count);
        return count;
    }

    //  could probably replace with a template that would call any lambda for all cells
    void NextGeneration()
    {
        _generation++;
        Cell::ResetCounts();

        for (int i = 0; i < _size; i++)
        {
            _board[i].NextGeneration();
        }
    }

    void RandomizeBoard(int n)
    {
        std::srand(std::time(nullptr));
        int rx, ry, ra;

        for (int z = 0; z < n; z++)
        {
            rx = std::rand() / ((RAND_MAX + 1u) / (Width() - 1));
            ry = std::rand() / ((RAND_MAX + 1u) / (Height() - 1));
            ra = std::rand() / ((RAND_MAX + 1u) / 99);

            Cell& cell = GetCell(rx, ry);
            if (cell.GetState() == Cell::State::Dead)
            {
                cell.SetState(Cell::State::Born);
            }
            NextGeneration();
        }
    }

    // does not work:
    //void UpdateBoard(std::function<void(Cell& cell)>& F)
    void UpdateBoard(auto F)
    {
        for (int y = 0; y < Height(); y++)
        {
            for (int x = 0; x < Width(); x++)
            {
                Cell& cc = GetCell(x, y);
                F(cc);
            }
        }
    }

    void ConwayFunction(Cell& cell)
    {
        // Any live cell with two or three live neighbours survives.
        // Any dead cell with three live neighbours becomes a live cell.
        // All other live cells die in the next generation. Similarly, all other dead cells stay dead.

        CountLiveNeighbors(cell);
        if (cell.IsAlive() && cell.Neighbors() >= 2 && cell.Neighbors() <= 3)
        {
            cell.SetState(Cell::State::Live);
        }
        else
        if (cell.IsDead() && cell.Neighbors() == 3)
        {
            cell.SetState(Cell::State::Born);
        }
        else
        if (cell.IsAlive())
        {
            cell.SetState(Cell::State::Dying);
        }
    }

    void Conway()
    {
        // Any live cell with two or three live neighbours survives.
        // Any dead cell with three live neighbours becomes a live cell.
        // All other live cells die in the next generation. Similarly, all other dead cells stay dead.
        // Cells that hit 100 'die' (this is currently commented out for testing)

        int count;
        for (int y = 0; y < Height(); y++)
        {
            for (int x = 0; x < Width(); x++)
            {
                Cell& cell = GetCell(x, y);
                count = CountLiveNeighbors(cell);

                if (cell.IsAlive() && count >= 2 && count <= 3)
                {
                    cell.SetState(Cell::State::Live);
                }
                else
                if (cell.IsDead() && count == 3)
                {
                    cell.SetState(Cell::State::Born);
                }
                else
                if (cell.IsAlive())
                {
                    cell.SetState(Cell::State::Dying);
                }
            }
        }
        NextGeneration();
    }

    void DayAndNight()
    {
        // https://en.wikipedia.org/wiki/Day_and_Night_(cellular_automaton)
        // rule notation B3678/S34678, meaning that a dead cell becomes live (is born)
        // if it has 3, 6, 7, or 8 live neighbors, and a live cell remains alive (survives)
        // if it has 3, 4, 6, 7, or 8 live neighbors,

        int count;
        for (int y = 0; y < Height(); y++)
        {
            for (int x = 0; x < Width(); x++)
            {
                Cell& cell = GetCell(x, y);
                count = CountLiveNeighbors(cell);

                if (cell.IsAlive() && ((count >= 3) && (count != 5)))
                {
                    cell.SetState(Cell::State::Live);
                }
                else
                if (cell.IsDead() && ((count == 3) || (count >= 6)))
                {
                    cell.SetState(Cell::State::Born);
                }
                else
                if (cell.IsAlive())
                {
                    cell.SetState(Cell::State::Dying);
                }
            }
        }
        NextGeneration();
    }

    void LifeWithoutDeath()
    {
        // https://en.wikipedia.org/wiki/Life_without_Death
        // every cell that was alive in the previous pattern remains alive,
        // every dead cell that has exactly 3 live neighbors becomes alive itself
        // and every other dead cell remains dead. B3/S012345678

        int count;
        for (int y = 0; y < Height(); y++)
        {
            for (int x = 0; x < Width(); x++)
            {
                Cell& cell = GetCell(x, y);
                count = CountLiveNeighbors(cell);

                if (cell.IsAlive())
                {
                    cell.SetState(Cell::State::Live);
                }
                else
                if (cell.IsDead() && count == 3)
                {
                    cell.SetState(Cell::State::Born);
                }
            }
        }
        NextGeneration();
    }

    void Highlife()
    {
        // https://en.wikipedia.org/wiki/Highlife_(cellular_automaton)
        // the rule B36 / S23; that is, a cell is born if it has 3 or 6 neighbors
        //and survives if it has 2 or 3 neighbors.

        int count;
        for (int y = 0; y < Height(); y++)
        {
            for (int x = 0; x < Width(); x++)
            {
                Cell& cell = GetCell(x, y);
                count = CountLiveNeighbors(cell);

                if (cell.IsAlive() && count >= 2 && count <= 3)
                {
                    cell.SetState(Cell::State::Live);
                }
                else
                if (cell.IsDead() && ((count == 3) || (count == 6)))
                {
                    cell.SetState(Cell::State::Born);
                }
                else
                if (cell.IsAlive())
                {
                    cell.SetState(Cell::State::Dying);
                }
            }
        }
        NextGeneration();
    }

    void Seeds()
    {
        // https://en.wikipedia.org/wiki/Seeds_(cellular_automaton)
        // In each time step, a cell turns on or is "born" if it was off or "dead"
        // but had exactly two neighbors that were on;
        // all other cells turn off. It is described by the rule B2 / S

        int count;
        for (int y = 0; y < Height(); y++)
        {
            for (int x = 0; x < Width(); x++)
            {
                Cell& cell = GetCell(x, y);
                count = CountLiveNeighbors(cell);

                if (cell.IsDead() && count == 2)
                {
                    cell.SetState(Cell::State::Born);
                }
                else
                {
                    cell.SetState(Cell::State::Dying);
                }
            }
        }
        NextGeneration();
    }

    void BriansBrain()
    {
        // https://en.wikipedia.org/wiki/Brian%27s_Brain
        // In each time step, a cell turns on if it was off but had exactly two neighbors that were on,
        // just like the birth rule for Seeds. All cells that were "on" go into the "dying" state,
        // which is not counted as an "on" cell in the neighbor count, and prevents any cell from
        // being born there. Cells that were in the dying state go into the off state.

        int count;
        for (int y = 0; y < Height(); y++)
        {
            for (int x = 0; x < Width(); x++)
            {
                Cell& cell = GetCell(x, y);
                count = CountLiveNotDyingNeighbors(cell);
                if (cell.IsDead() && count == 2)
                {
                    cell.SetState(Cell::State::Born);
                }
                else
                if (cell.GetState() == Cell::State::Live)
                {
                    cell.SetState(Cell::State::Dying);
                }
                else
                if (cell.GetState() == Cell::State::Dying)
                {
                    cell.SetState(Cell::State::Dead);
                }
            }
        }
        NextGeneration();
    }
};

std::wostream& operator<<(std::wostream& stream, Board& board)
{
    //static std::vector<std::wstring> str;
    std::wstring str;
    str.reserve((board.Width() + 1) * board.Height());
    str = L"\0";
    for (int y = 0; y < board.Height(); y++)
    {
        for (int x = 0; x < board.Width(); x++)
        {
            Cell& cell = board.GetCell(x, y);
            str += cell.GetWideStateString();
        }
        str += L"\n";
    }

    wprintf(str.c_str());
    return stream;
}

//std::ostream& operator<<(std::ostream& os, const std::u8string& str)
//{
//    os << reinterpret_cast<const char*>(str.data());
//    return os;
//}

std::ostream& operator<<(std::ostream& stream, Board& board)
{
    //static std::vector<std::wstring> str;
    std::u8string str;
    str.reserve((board.Width() + 1) * board.Height());
    str = u8"\0\0";
    for (int y = 0; y < board.Height(); y++)
    {
        for (int x = 0; x < board.Width(); x++)
        {
            Cell& cell = board.GetCell(x, y);
            str += cell.GetEmojiStateString();
        }
        str += u8"\n";
    }
    str += u8"\0";

    printf((const char*)str.c_str());
    return stream;
}

int main()
{
    auto UTF8 = std::locale("en_US.UTF-8");
    std::locale::global(UTF8);
    std::wcout.imbue(UTF8);
    std::cout.imbue(UTF8);
    setlocale(LC_ALL, "en_us.utf8");
    SetConsoleOutputCP(CP_UTF8);

#ifdef _DEBUG
    std::wcout << L"Getting console" << std::endl;
#endif

    // Set output mode to handle virtual terminal sequences
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE)
    {
        return false;
    }

    DWORD dwOriginalOutMode = 0;
    if (!GetConsoleMode(hOut, &dwOriginalOutMode))
    {
        return false;
    }

    DWORD dwRequestedOutModes = ENABLE_VIRTUAL_TERMINAL_PROCESSING /*| DISABLE_NEWLINE_AUTO_RETURN*/;

    DWORD dwOutMode = dwOriginalOutMode | dwRequestedOutModes;
    if (!SetConsoleMode(hOut, dwOutMode))
    {
        // we failed to set both modes, try to step down mode gracefully.
        dwRequestedOutModes = ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        dwOutMode = dwOriginalOutMode | dwRequestedOutModes;
        if (!SetConsoleMode(hOut, dwOutMode))
        {
            // Failed to set any VT mode, can't do anything here.
            return -1;
        }
    }

    // could ask the user for board size
    // and at what age, if any, they want to old cells to die
    // TODO make board._board a heap allocated variable (it's the big thing)

    CONSOLE_SCREEN_BUFFER_INFO csbi = {};
    GetConsoleScreenBufferInfo(hOut, &csbi);
#ifdef _DEBUG
    std::wcout << L"Setting up board" << std::endl;
    Board board(60, 30);
#else
    Board board(csbi.dwSize.X/2, csbi.dwSize.Y - 5);
#endif

    // Randomly fill  spots for n 'generations'
	int n = board.Width() * board.Height() / 4;
#ifdef _DEBUG
    std::wcout << L"Randomly populating cells for " << n << " generations" << std::endl;
#endif
	board.RandomizeBoard(n);
#ifdef _DEBUG
    std::wcout << L"\x1b[mHit <enter> for next generation, 'n' to stop" << std::endl << std::endl;
    if (std::cin.get() == 'n')
        return 0;
#endif

    // using example from https://en.cppreference.com/w/cpp/utility/functional/bind
    // auto f3 = std::bind(&Foo::print_sum, &foo, 95, _1);

    //this works
    //void UpdateBoard(auto F)
    //void ConwayFunction(Cell & cell)
    //auto C = std::bind(&Board::ConwayFunction, &board, std::placeholders::_1);

    //this works too
    //std::function<void(Cell& cell)> C = [&](Cell& cell) -> void
    //{
    //    board.ConwayFunction(cell);
    //};

    //this should work but doesn't
    // https://en.cppreference.com/w/cpp/utility/functional/mem_fn
    //auto C = std::mem_fn(&Board::ConwayFunction);

    auto C = std::bind(&Board::ConwayFunction, &board, std::placeholders::_1);
    system("CLS"); // Windows only

    COORD coordScreen = { 0, 0 };
    //\x1b[31mo
    std::cout << "\x1b[?25l";
    // simulation loop
    const int msSleep = 1;
    while (true)
	{
#ifdef _DEBUG
        //Sleep(1000);
#else
        Sleep(msSleep);
#endif
        SetConsoleCursorPosition(hOut, coordScreen);

        std::cout << "\x1b[mGeneration " << board.Generation() << ". Alive: " << Cell::GetLiveCount() << " Dead: " << Cell::GetDeadCount() << " Born: " << Cell::GetBornCount() << " Dying: " << Cell::GetDyingCount() << "                     " << std::endl;
#ifdef _DEBUG
        std::wcout << L"\x1b[mHit <enter> for next generation, 'n' to stop" << std::endl << std::endl;
#endif
        std::cout << board << std::endl;

#ifdef _DEBUG
        if (std::cin.get() == 'n')
			break;
#endif

        board.UpdateBoard(C);

        // this code lets you see the transitions, comment it out if just want to see the generations
        Sleep(msSleep);
        SetConsoleCursorPosition(hOut, coordScreen);
            std::cout << "\x1b[mGeneration " << board.Generation() << ". Alive: " << Cell::GetLiveCount() << " Dead: " << Cell::GetDeadCount() << " Born: " << Cell::GetBornCount() << " Dying: " << Cell::GetDyingCount() << "                     " << std::endl;
            //std::cout << "\x1b[mHit <enter> for next generation, 'n' to stop" << std::endl << std::endl;
            std::cout << board << std::endl;

            ////if (std::cin.get() == 'n')
            ////    break;
        // end of transitions code

        board.NextGeneration();

        //Old way where the loop is copy pasted everywhere
        //board.Conway();
		//board.Seeds();
		//board.BriansBrain();
		//board.Highlife();
		//board.LifeWithoutDeath();
		//board.DayAndNight();
	}
	std::cout << "\x1b[mThanks for the simulation" << std::endl;
}

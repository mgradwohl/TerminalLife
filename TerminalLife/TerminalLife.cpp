// TerminalLife.cpp
#include "pch.h"

class Cell
{
public:
    // in many implementations a new copy of the board is made
    // and the new board is made by looking at the old board.
    // I do this with five states: Dead, Born, Live, Old, Dying
    // Born means "will be alive in the next generation"
    // Dying means "will die in the next generation"
    // Old means "about to die" if lifespans of cells are enabled
    // an advantage is that the board never changes size after instantiation
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
    Cell() : _state(State::Dead), _age(0), _x(0), _y(0), _neighbors(0)
    {
    }

    Cell(int x, int y) : _state(State::Dead), _age(0), _x(x), _y(y), _neighbors(0)
    {
    }

    Cell(State state) : _state(state), _age(0), _x(0), _y(0), _neighbors(0)
    {
        if (_state == Cell::State::Born) _age = 0;
    }

    // the compiler will not let me delete the copy constructor, AND when I implement them
    // they don't seem to be called
    Cell(const Cell& c)
        : _state(c._state), _age(c._age), _x(c._x), _y(c._y), _neighbors(c._y)
    {
        std::cout << "Cell const copy constructor" << std::endl;
        std::cin.get();
    }

    Cell(Cell& c)
        : _state(c._state), _age(c._age), _x(c._x), _y(c._y), _neighbors(c._y)
    {
        std::cout << "Cell copy constructor" << std::endl;
        std::cin.get();
    }

    ~Cell() = default;
    Cell const& operator=(Cell& cell) = delete;
    //Cell(Cell& cell) = delete;

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

    void SetState(State state)
    {
        _state = state;

        switch (_state)
        {
            case Cell::State::Dead:
            {
                numDead++;
                _age = 0;
                break;
            }
            case Cell::State::Live: numLive++;
                break;
            case Cell::State::Born:
            {
                numBorn++;
                _age = 0;
                break;
            }
            case Cell::State::Old: numOld++;
                break;
            case Cell::State::Dying: numDying++;
                break;
            default: 
                // do nothing
                break;
        }
    }

    State GetState() const
    {
        return _state;
    }

    bool IsAlive() const
    {
        if (_state == Cell::State::Live || _state == Cell::State::Dying || _state == Cell::State::Old)
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
        case State::Old: return "x";
            break;
        case State::Dying: return ".";
            break;
        default:
            return "?";
        }
    }

    auto GetEmojiStateString() const
    {
        //static const std::wstring strDead(L" ");
        //static const std::wstring strLive(L"\x1b[mO");
        //static const std::wstring strBorn(L"\x1b[32mo");
        //static const std::wstring strDying(L"\x1b[31mo");
        //static const std::wstring strOld(L"\x1b[31mx");
        //static const std::wstring strUnkown(L"\x1b[31m?");

        static auto cDead    = u8"🖤\0";
        static auto cLive    = u8"😁\0";
        static auto cBorn    = u8"💕\0";
        static auto cOld     = u8"🤡\0";
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
            case State::Old: return cOld;
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
    }

    void KillOldCell()
    {
        // we only enforce this rule if age > 0
        if (Cell::OldAge > 0)
        {
            if (_age ==  Cell::OldAge - 2)
            {
                // mark it as old, about to die
                SetState(Cell::State::Old);
            }
            else if (_age == Cell::OldAge - 1)
            {
                // mark it as old, about to die
                SetState(Cell::State::Dying);
            }
            else if (_age >= Cell::OldAge)
            {
                SetState(Cell::State::Dead);
            }
        }
    }
};

// for visualization purposes (0,0) is the top left.
// as x increases move right, as y increases move down
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

    //Board(const Board& b)
    //    : _width(b._width), _height(b._height), _size(b._size), _generation(b._generation), _x(b._x), _y(b._y)
    //{
    //}

    Board(Board& b) = delete;
    ~Board() = default;
    Board const& operator=(Board& b) = delete;

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
        Cell& cell = GetCell(x,y);
        cell.SetState(state);
    }

    const Cell& GetCell(int x, int y) const
    {
        // no bounds checking
        return _board[x + (y * _width)];
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
        static int x = 0;
        static int y = 0;
        x = cell.X();
        y = cell.Y();

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
        static int x = 0;
        static int y = 0;
        x = cell.X();
        y = cell.Y();

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
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> xdis(0, _width-1);
        std::uniform_int_distribution<> ydis(0, _height-1);
        std::uniform_int_distribution<> adis(0, 100);

        int rx, ry, ra;

        for (int z = 0; z < n; z++)
        {
            rx = xdis(gen);
            ry = ydis(gen);
            ra = adis(gen);

            Cell& cell = GetCell(rx, ry);
            if (cell.GetState() == Cell::State::Dead)
            {
                cell.SetState(Cell::State::Born);
                cell.SetAge(ra);
            }
            NextGeneration();
        }
    }

    // This form does not work: void UpdateBoard(std::function<void(Cell& cell)>& F)
    // but using auto is magic
    void UpdateBoard(auto F)
    {
        for (int y = 0; y < Height(); y++)
        {
            for (int x = 0; x < Width(); x++)
            {
                Cell& cc = GetCell(x, y);
                CountLiveNeighbors(cc);
                F(cc);
                cc.KillOldCell();
            }
        }
    }

    void ConwayRules(Cell& cell) const
    {
        // Any live cell with two or three live neighbours survives.
        // Any dead cell with three live neighbours becomes a live cell.
        // All other live cells die in the next generation. Similarly, all other dead cells stay dead.

        static int count = 0;
        count = cell.Neighbors();

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

    void DayAndNightRules(Cell& cell) const
    {
        // https://en.wikipedia.org/wiki/Day_and_Night_(cellular_automaton)
        // rule notation B3678/S34678, meaning that a dead cell becomes live (is born)
        // if it has 3, 6, 7, or 8 live neighbors, and a live cell remains alive (survives)
        // if it has 3, 4, 6, 7, or 8 live neighbors,

        static int count = 0;
        count = cell.Neighbors();

        if (cell.IsAlive() && ((count >= 3) && (count != 5)))
        {
            cell.SetState(Cell::State::Live);
        }
        else if (cell.IsDead() && ( count == 3 || count >= 6 ))
        {
            cell.SetState(Cell::State::Born);
        }
        else if (cell.IsAlive())
        {
            cell.SetState(Cell::State::Dying);
        }
    }

    void LifeWithoutDeathRules(Cell& cell) const
    {
        // https://en.wikipedia.org/wiki/Life_without_Death
        // every cell that was alive in the previous pattern remains alive,
        // every dead cell that has exactly 3 live neighbors becomes alive itself
        // and every other dead cell remains dead. B3/S012345678

        static int count = 0;
        count = cell.Neighbors();

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

    void HighlifeRules(Cell& cell) const
    {
        // https://en.wikipedia.org/wiki/Highlife_(cellular_automaton)
        // the rule B36 / S23; that is, a cell is born if it has 3 or 6 neighbors
        //and survives if it has 2 or 3 neighbors.

        static int count = 0;
        count = cell.Neighbors();

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

    void SeedsRules(Cell& cell) const
    {
        // https://en.wikipedia.org/wiki/Seeds_(cellular_automaton)
        // In each time step, a cell turns on or is "born" if it was off or "dead"
        // but had exactly two neighbors that were on
        // all other cells turn off. It is described by the rule B2 / S

        static int count = 0;
        count = cell.Neighbors();

        if (cell.IsDead() && count == 2)
        {
            cell.SetState(Cell::State::Born);
        }
        else
        {
            cell.SetState(Cell::State::Dying);
        }
    }

    void BriansBrainRules(Cell& cell) const
    {
        // https://en.wikipedia.org/wiki/Brian%27s_Brain
        // In each time step, a cell turns on if it was off but had exactly two neighbors that were on,
        // just like the birth rule for Seeds. All cells that were "on" go into the "dying" state,
        // which is not counted as an "on" cell in the neighbor count, and prevents any cell from
        // being born there. Cells that were in the dying state go into the off state.

        static int count = 0;
        count = cell.Neighbors();

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
};

std::ostream& operator<<(std::ostream& stream, Board& board)
{
    static std::u8string str( ((board.Width() + 2) * board.Height()) + 1, ' ');
    str.clear();
    
    for (int y = 0; y < board.Height(); y++)
    {
        for (int x = 0; x < board.Width(); x++)
        {
            const Cell& cell = board.GetCell(x, y);
            str += cell.GetEmojiStateString();
        }
        str += u8"\r\n";
    }
    str += u8"\0";

    printf((const char*)str.c_str());
    return stream;
}

int main()
{
    // make sure Windows Console and C++ runtime are set for utf8
    auto UTF8 = std::locale("en_US.UTF-8");
    std::locale::global(UTF8);
    std::cout.imbue(UTF8);
    setlocale(LC_ALL, "en_us.utf8");
    SetConsoleOutputCP(CP_UTF8);

    // don't sync the C++ streams to the C streams, for performance
    std::ios::sync_with_stdio(false);

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

    system("CLS");
    std::cout << "\x1b[mWelcome to TerminalLife, resize your console to get the biggest simulation, then press [ENTER]" << std::endl;
    std::cout << "\x1b[mHit ENTER to start, SPACE to pause/unpause, ESC to quit, [+] and [-] to change speed, [S] to show details, [F] to show cell fates, and [I] to toggle incremental vs. continuous simulation" << std::endl;
    std::cin.get();

    CONSOLE_SCREEN_BUFFER_INFO csbi = {};
    GetConsoleScreenBufferInfo(hOut, &csbi);
#ifdef _DEBUG
    // small boards are easier for debugging
    Board board(60, 30);
#else
    Board board(csbi.dwSize.X/2, csbi.dwSize.Y - 5);
#endif

    // Rulesets
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

    std::function<void(Cell& cell)> C = [&](Cell& cell)-> void
    {
        board.ConwayRules(cell);
    };

    //auto C = std::bind(&Board::ConwayRules, &board, std::placeholders::_1);
    auto D = std::bind(&Board::DayAndNightRules, &board, std::placeholders::_1);
    auto S = std::bind(&Board::SeedsRules, &board, std::placeholders::_1);
    auto B = std::bind(&Board::BriansBrainRules, &board, std::placeholders::_1);
    auto H = std::bind(&Board::HighlifeRules, &board, std::placeholders::_1);
    auto L = std::bind(&Board::LifeWithoutDeathRules, &board, std::placeholders::_1);

    // Randomly fill  spots for n 'generations'
	int n = board.Width() * board.Height() / 4;
    std::cout << "Randomly populating cells for " << n << " generations" << std::endl;
	board.RandomizeBoard(n);

    system("CLS"); // Windows only
    COORD coordScreen = { 0, 0 };

    // turn off the cursor
    std::cout << "\x1b[?25l" << std::endl;

    //untie cin and cout, since we won't use cin anymore and this improves performance
    std::cin.tie(nullptr);

    // simulation loop
#ifdef _DEBUG
    int msSleep = 300;
    bool fFate = true;
    bool fScore = true;
    bool fIncremental = true;
    bool fOldAge = true;
#else
    int msSleep = 0;
    bool fFate = false;
    bool fScore = false;
    bool fIncremental = false;
    bool fOldAge = false;
#endif

    // used for an attempt to ensure that keystrokes in other apps don't impact this one
    //HWND hWndC = GetActiveWindow();
    //HWND hWndF = GetForegroundWindow();
    while (true)
    {
        //if (GetConsoleWindow() == GetFocus())
        //{
            if (GetAsyncKeyState(VK_ESCAPE) & 0x01)
            {
                break;
            }

            if (GetAsyncKeyState(VK_OEM_PLUS) & 0x01)
                msSleep += 100;

            if (GetAsyncKeyState(VK_F1) & 0x01)
                fOldAge = !fOldAge;

            if (GetAsyncKeyState(VK_OEM_MINUS) & 0x01)
            {
                msSleep -= 100;
                if (msSleep < 1) msSleep = 0;
            }

            if (GetAsyncKeyState(0x46) & 0x01)
                fFate = !fFate;

            if (GetAsyncKeyState(0x53) & 0x01)
                fScore = !fScore;

            if (GetAsyncKeyState(0x49) & 0x01)
                fIncremental = !fIncremental;
        //}

        if (fOldAge)
        {
            Cell::SetOldAge(80);
        }
        else
        {
            Cell::SetOldAge(-1);
        }

        SetConsoleCursorPosition(hOut, coordScreen);

        if (fScore)
        {
            std::cout << "\x1b[mGeneration " << board.Generation() << ". Sleep: " << msSleep << ". Life Span: " << fOldAge << ". Alive: " << Cell::GetLiveCount() << ". Dead: " << Cell::GetDeadCount() << ". Born: " << Cell::GetBornCount() << ". Dying: " << Cell::GetDyingCount() << ". OldAge: " << Cell::GetOldCount() << ".\x1b[0K\n";
        }
        else std::cout << "\x1b[2K\n";

        if (fIncremental)
        {
            std::cout << "\x1b[mHit SPACE for next screen, [I] to continuously update\n\n" ;
        }
        else std::cout << "\x1b[2K\n";

        // print the board to the console AND flush the stream
        std::cout << board << std::endl;

        if (fIncremental)
        {
            bool Paused = true;
            while (Paused)
            {
                if (GetAsyncKeyState(VK_SPACE) & 0x01)
                {
                    Paused = false;
                }  
            }
        }
        else Sleep(msSleep);

        // PICK YOUR RULESET HERE
        // this calls a Function with rules that determine the state of the cells in the next generation, without changing the cells
        board.UpdateBoard(H);

        // this will show the user the pending changes to the board (born, dying, etc.)
        if (fFate)
        {
            SetConsoleCursorPosition(hOut, coordScreen);
            if (fScore)
            {
                std::cout << "\x1b[mGeneration " << board.Generation() << ". Sleep: " << msSleep << ". Life Span: " << fOldAge << ". Alive: " << Cell::GetLiveCount() << ". Dead: " << Cell::GetDeadCount() << ". Born: " << Cell::GetBornCount() << ". Dying: " << Cell::GetDyingCount() << ". OldAge: " << Cell::GetOldCount() << ".\x1b[0K\n";
            }
            else std::cout << "\x1b[2K\n";

            if (fIncremental)
            {
                 std::cout << "\x1b[mHit SPACE for next screen, [I] to continuously update\n\n";
            }
            else std::cout << "\x1b[2K\n";

            // print the board with Fates to the console AND flush the stream
            std::cout << board << std::endl;

            if (fIncremental)
            {
                bool Paused = true;
                while (Paused)
                {
                    if (GetAsyncKeyState(VK_SPACE) & 0x01)
                    {
                        Paused = false;
                    }  
                }
            }
            else Sleep(msSleep);
        }

        // this applies the changes that were determined by the ruleset called by Board::UpdateBoard();
        board.NextGeneration();
    }

    std::cout << "\x1b[mThanks for the simulation" << std::endl;
    SetConsoleMode(hOut, dwOriginalOutMode);
}
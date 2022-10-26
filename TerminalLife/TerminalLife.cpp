// TerminalLife.cpp
#include "pch.h"
#include "cell.h"
#include "ConsoleConfig.h"

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
    Board(const Board& b) = delete;
    Board(Board& b) = delete;
    ~Board() = default;
    Board const& operator=(Board& b) = delete;

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

// optiimzed to never use std::endl until the full board is done printing
std::ostream& operator<<(std::ostream& stream, Board& board)
{
    static std::u8string str( ((board.Width() + 2) * board.Height()) + 1, ' ');
    // clear the static string of any leftover goo
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

    printf((const char*)str.c_str());
    return stream;
}


//DrawOptions DrawOptions::s_Instance;

class DrawOptions
{
public:
    static DrawOptions& Get()
    {
        static DrawOptions s_Instance;
        return s_Instance;
    }

    DrawOptions() {}
    DrawOptions(const DrawOptions&) = delete;

    ~DrawOptions() {}


private:
    static DrawOptions s_Instance;

    // initial options
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

    bool CheckKeyState()
    {
        if (GetAsyncKeyState(VK_ESCAPE) & 0x01)
        {
            return false;
        }

        if (GetAsyncKeyState(VK_OEM_PLUS) & 0x01)
            _msSleep += 100;

        if (GetAsyncKeyState(VK_F1) & 0x01)
            _fOldAge = !_fOldAge;

        if (GetAsyncKeyState(VK_OEM_MINUS) & 0x01)
        {
            _msSleep -= 100;
            if (_msSleep < 1) _msSleep = 0;
        }

        if (GetAsyncKeyState(0x46) & 0x01)
            _fFate = !_fFate;

        if (GetAsyncKeyState(0x53) & 0x01)
            _fScore = !_fScore;

        if (GetAsyncKeyState(0x49) & 0x01)
            _fIncremental = !_fIncremental;

        return true;
    }
};

int main()
{
    ConsoleConfig console;
    if (!console.ConsoleInit())
        exit(-1);

    console.PrintIntro();

    // small boards are easier for debugging
    #ifdef _DEBUG
        Board board(60, 30);
    #else
        Board board(console.Width() / 2, console.Height() - 10);
    #endif

    // Randomly fill  spots for n 'generations'
    int n = board.Width() * board.Height() / 4;
    std::cout << "Randomly populating cells for " << n << " generations" << std::endl;
    board.RandomizeBoard(n);
        
    // Rulesets
    auto C = std::bind(&Board::ConwayRules, &board, std::placeholders::_1);
    auto D = std::bind(&Board::DayAndNightRules, &board, std::placeholders::_1);
    auto S = std::bind(&Board::SeedsRules, &board, std::placeholders::_1);
    auto B = std::bind(&Board::BriansBrainRules, &board, std::placeholders::_1);
    auto H = std::bind(&Board::HighlifeRules, &board, std::placeholders::_1);
    auto L = std::bind(&Board::LifeWithoutDeathRules, &board, std::placeholders::_1);

    // pick your Ruleset here
    auto Ruleset = C;

    console.DrawBegin();
    
    // simulation loop
    while (true)
    {
        console.SetPositionHome();

        if (!DrawOptions::Get().CheckKeyState())
            break;// exit(0);

        // DrawOptions::Get().PrintBoardHeader();
        // would like to move these into Board or DrawOptions
        if (DrawOptions::Get().Score())
        {
            std::cout << "\x1b[mGeneration " << board.Generation() << ". Sleep: " << DrawOptions::Get().Delay() << ". Life Span: " << DrawOptions::Get().OldAge() << ". Alive: " << Cell::GetLiveCount() << ". Dead: " << Cell::GetDeadCount() << ". Born: " << Cell::GetBornCount() << ". Dying: " << Cell::GetDyingCount() << ". OldAge: " << Cell::GetOldCount() << ".\x1b[0K\n";
        }
        else std::cout << "\x1b[2K\n";

        if (DrawOptions::Get().Incremental())
        {
            std::cout << "\x1b[mHit SPACE for next screen, [I] to continuously update\n\n" ;
        }
        else std::cout << "\x1b[2K\n";

        // print the board to the console AND flush the stream
        console.SetPositionBoard();
        std::cout << board << std::endl;

        if (DrawOptions::Get().Incremental())
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
        else Sleep(DrawOptions::Get().Delay());

        Cell::SetOldAge(DrawOptions::Get().OldAge());
        board.UpdateBoard(Ruleset);

        // this will show the user the pending changes to the board (born, dying, etc.)
        if (DrawOptions::Get().Fate())
        {
            console.SetPositionHome();
            if (DrawOptions::Get().Score())
            {
                std::cout << "\x1b[mGeneration " << board.Generation() << ". Sleep: " << DrawOptions::Get().Delay()  << ". Life Span: " << DrawOptions::Get().OldAge() << ". Alive: " << Cell::GetLiveCount() << ". Dead: " << Cell::GetDeadCount() << ". Born: " << Cell::GetBornCount() << ". Dying: " << Cell::GetDyingCount() << ". OldAge: " << Cell::GetOldCount() << ".\x1b[0K\n";
            }
            else std::cout << "\x1b[2K\n";

            if (DrawOptions::Get().Incremental())
            {
                 std::cout << "\x1b[mHit SPACE for next screen, [I] to continuously update\n\n";
            }
            else std::cout << "\x1b[2K\n";

            // print the board with Fates to the console AND flush the stream
            console.SetPositionBoard();
            std::cout << board << std::endl;

            if (DrawOptions::Get().Incremental())
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
            else Sleep(DrawOptions::Get().Delay());
        }

        // this applies the changes that were determined by the ruleset called by Board::UpdateBoard();
        board.NextGeneration();
    }

    console.Clear();
    std::cout << "\x1b[mThanks for the simulation" << std::endl;

    // console dtor will restore the console
}
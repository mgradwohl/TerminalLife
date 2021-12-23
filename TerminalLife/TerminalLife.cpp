// TerminalLife.cpp
#include <iostream>
#include <vector>
#include <functional>

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
    int _x, _y, _age, _n;
    State _state;

public:
    Cell()
        : _state(State::Dead), _age(0), _x(0), _y(0), _n(0)
    {
        //std::wcout << "Cell constructor Cell()" << std::endl;
    }

    Cell(int x, int y)
        : _state(State::Dead), _age(0), _x(x), _y(y), _n(0)
    {
        //std::wcout << "Cell constructor Cell(x,y)" << std::endl;
    }

    Cell(State state)
        : _state(state), _age(0), _x(0), _y(0), _n(0)
    {
        if (_state == Cell::State::Born) _age = 0;
        //std::wcout << "Cell constructor Cell(state)" << std::endl;
    }

    Cell(const Cell& c)
        : _state(c._state), _age(c._age), _x(c._x), _y(c._y), _n(c._y)
    {
        std::wcout << "Cell copy constructor" << std::endl;
    }

    ~Cell()
    {}

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

    int N() const
    {
        return _n;
    }

    void SetN(int n)
    {
        _n = n;
    }

    int Age() const
    {
        return _age;
    }

    void SetState(State state)
    {
        _state = state;
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

    // Not sure why this doesn't work. GRAFIX!!! would be cool
    //const wchar_t* GetWideStateString()
    //{
    //    switch (_state)
    //    {
    //    case State::Dead: return L"💩";
    //        break;
    //    case State::Born: return L"👼";
    //        break;
    //    case State::Live: return L"😁";
    //        break;
    //    case State::Dying: return L"💔";
    //        break;
    //    default:
    //        return L"⁉️";
    //    }
    //}

    const wchar_t* GetWideStateString() const
    {
        switch (_state)
        {
        case State::Dead: return L" ";
            break;
        case State::Born: return L"o";
            break;
        case State::Live: return L"O";
            break;
        case State::Dying: return L".";
            break;
        default:
            return L"⁉️";
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
            _state = Cell::State::Live;
        }

        if (_state == Cell::State::Dying)
        {
            _state = Cell::State::Dead;
        }

        _age++;
        // Set a positive int to kill old cells when they reach age==int
        KillOldCells(-1);
    }

    void NextGenerationBrain()
    {
        if (_state == Cell::State::Dead)
        {
            // no birthday for you
            return;
        }

        if (_state == Cell::State::Born)
        {
            _state = Cell::State::Live;
        }

        if (_state == Cell::State::Dying)
        {
            _state = Cell::State::Dead;
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
            _state = Cell::State::Dead;
    }
};

typedef std::vector<std::vector<Cell> > Vector2D;

// for visualization purposes (0,0) is the top left.
// as x increases move right
// as y increases move down
class Board
{
private:
    // if I allocated this on the heap, I could get the size right with resize
    Vector2D _board;
    int _width, _height;
    int _generation;
    int _x, _y;

public:
    Board(int width, int height)
        : _width(width), _height(height), _generation(0), _x(0), _y(0)
    {
        std::wcout << "Board constructor Board(w, h)" << std::endl;
        _board.resize(_height);
        for (int y = 0; y < _height; y++)
        {
            _board[y].resize(width);
            for (int x = 0; x < _width; x++)
            {
                // gotta be a better way?
                _board[y][x].SetXY(x, y);
            }
        }
    }

    Board(const Board& b)
        : _width(b._width), _height(b._height), _generation(b._generation), _x(b._x), _y(b._y)
    {
        std::wcout << "Board copy constructor" << std::endl;
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
        _board[y][x] = state;
    }

    Cell& GetCell(int x, int y)
    {
        // no bounds checking
        return _board[y][x];
    }

    Cell& CurrentCell()
    {
        return _board[_y][_x];
    }

    // used these for debugging
    int ClampX(int x) const
    {
        if (x < 0)
            x = 0;
        if (x >= _width)
            x = _width - 1;

        return x;
    }

    int ClampY(int y) const
    {
        if (y < 0)
            y = 0;
        if (y >= _height)
            y = _height - 1;

        return y;
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

        cell.SetN(count);

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

        cell.SetN(count);
        return count;
    }

    //  could probably replace with a template that would call any lambda for all cells
    void NextGeneration()
    {
        _generation++;
        for (int y = 0; y < Height(); y++)
        {
            for (int x = 0; x < Width(); x++)
            {
                GetCell(x, y).NextGeneration();
            }
        }
    }

    void NextGenerationBrain()
    {
        _generation++;
        for (int y = 0; y < Height(); y++)
        {
            for (int x = 0; x < Width(); x++)
            {
                GetCell(x, y).NextGenerationBrain();
            }
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
        NextGeneration();
    }

    void ConwayFunction(Cell& cell)
    {
        // Any live cell with two or three live neighbours survives.
        // Any dead cell with three live neighbours becomes a live cell.
        // All other live cells die in the next generation. Similarly, all other dead cells stay dead.

        CountLiveNeighbors(cell);
        if (cell.IsAlive() && cell.N() >= 2 && cell.N() <= 3)
        {
            cell.SetState(Cell::State::Live);
        }
        else
        if (cell.IsDead() && cell.N() == 3)
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
        NextGenerationBrain();
    }
};

std::ostream& operator<<(std::ostream& stream, Cell& cell)
{
    std::cout << cell.GetStateString();
    return stream;
}

std::wostream& operator<<(std::wostream& stream, Cell& cell)
{
    std::wcout << cell.GetWideStateString();
    return stream;
}

std::ostream& operator<<(std::ostream& stream, Board& board)
{
    for (int y = 0; y < board.Height(); y++)
    {
        for (int x = 0; x < board.Width(); x++)
        {
            Cell& cell = board.GetCell(x, y);
            std::cout << cell;
        }
        std::cout << std::endl;
    }
    return stream;
}

std::wostream& operator<<(std::wostream& stream, Board& board)
{
    for (int y = 0; y < board.Height(); y++)
    {
        for (int x = 0; x < board.Width(); x++)
        {
            Cell& cell = board.GetCell(x, y);
            std::wcout << cell;
        }
        std::wcout << std::endl;
    }
    return stream;
}

int main()
{
	// could ask the user for board size
	// and at what age, if any, they want to old cells to die
    // TODO make board._board a heap allocated variable (it's the big thing)
    Board board(75, 50);

    // Randomly fill  spots for n 'generations'
	int n = board.Width() * board.Height() / 2;
	std::wcout << L"Randomly populating cells for " << n << " generations" << std::endl;
	board.RandomizeBoard(n);

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

    // simulation loop
    while (true)
	{
		// this clears the screen so the board draws over itself
		std::wcout << L"\x1B[2J\x1B[H";
        //system("CLS"); // Windows only

		std::wcout << L"Generation " << board.Generation() << std::endl;
		std::wcout << L"Hit <enter> for next generation, 'n' to stop" << std::endl << std::endl;
		std::wcout << board << std::endl;

		if (std::cin.get() == 'n')
			break;

        board.UpdateBoard(C);

        //Old way where the loop is copy pasted everywhere
        //board.Conway();
		//board.Seeds();
		//board.BriansBrain();
		//board.Highlife();
		//board.LifeWithoutDeath();
		//board.DayAndNight();
	}
	std::wcout << L"Thanks for the simulation" << std::endl;
}

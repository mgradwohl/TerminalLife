// TerminalLife.cpp : This file contains the 'main' function. Program execution begins and ends there.
#include <iostream>
#include <vector>

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
    int _x, _y;
      State _state;
    int _age;

public:
    Cell()
        : _state(State::Dead), _age(0), _x(0), _y(0)
    {}

    Cell(int x, int y)
        : _state(State::Dead), _age(0), _x(x), _y(y)
    {}

    Cell(State state)
        : _state(state), _age(0), _x(0), _y(0)
    {
        if (_state == Cell::State::Born) _age = 0;
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

    //Not sure why this doesn't work. GRAFIX!!! would be cool
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

    int GetAge() const
    {
        return _age;
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
    Vector2D _board;
    int _width, _height;
    int _generation;

public:
    Board(int width, int height)
        : _width(width), _height(height), _generation(0)
    {
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

    int GetGeneration() const
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

    int CountNeighbors(Cell& cell)
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

    void TerminalConway()
    {
        //Any live cell with two or three live neighbours survives.
        //Any dead cell with three live neighbours becomes a live cell.
        //All other live cells die in the next generation. Similarly, all other dead cells stay dead.
        //Cells that hit 100 'die' (this is currently commented out for testing)

        int count;
        for (int y = 0; y < Height(); y++)
        {
            for (int x = 0; x < Width(); x++)
            {
                Cell& cell = GetCell(x, y);
                count = CountNeighbors(cell);

                if (cell.IsAlive() && count >= 2 && count <= 3)
                {
                    cell.SetState(Cell::State::Live);
                    //std::wcout << L"Cell at " << x << ", " << y << " lives" << std::endl;
                }
                else
                    if (cell.IsDead() && count == 3)
                    {
                        cell.SetState(Cell::State::Born);
                        //std::wcout << L"Cell at " << x << ", " << y << " born" << std::endl;
                    }
                    else
                        if (cell.IsAlive())
                        {
                            cell.SetState(Cell::State::Dying);
                            //std::wcout << L"Cell at " << x << ", " << y << "  dies" << std::endl;
                        }
            }
        }
        NextGeneration();
    }
};

std::ostream& operator<<(std::ostream& stream, Cell& cell)
{
    std::wcout << cell.GetStateString();
    return stream;
}

std::wostream& operator<<(std::wostream& stream, Cell& cell)
{
    std::wcout << cell.GetWideStateString();
    return stream;
}

std::ostream& operator<<(std::ostream& stream, Board& board)
{
    Cell cell;
    for (int y = 0; y < board.Height(); y++)
    {
        for (int x = 0; x < board.Width(); x++)
        {
            cell = board.GetCell(x, y);
            std::cout << cell;
        }
        std::cout << std::endl;
    }
    return stream;
}

std::wostream& operator<<(std::wostream& stream, Board& board)
{
    Cell cell;
    for (int y = 0; y < board.Height(); y++)
    {
        for (int x = 0; x < board.Width(); x++)
        {
            cell = board.GetCell(x, y);
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
    // if you make a big board, do it on the heap
    auto board = std::make_unique<Board>(75,50);

    // Randomly fill  spots for n 'generations'
	int n = board->Width() * board->Height() / 2;
	std::wcout << L"Randomly populating cells for " << n << " generations" << std::endl;
	board->RandomizeBoard(n);

	while (true)
	{
		// this clears the screen so the board draws over itself
        // they work the same, CLS is for Windows only
		std::wcout << L"\x1B[2J\x1B[H";
        //system("CLS");

		std::wcout << L"Generation " << board->GetGeneration() << std::endl;
		std::wcout << L"Hit <enter> for next generation, 'n' to stop" << std::endl << std::endl;
		std::wcout << (*board) << std::endl;

		if (std::cin.get() == 'n')
			break;

		board->TerminalConway();
	}
	std::wcout << L"Thanks for the simulation" << std::endl;
}

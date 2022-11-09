// TerminalLife.cpp
#include "pch.h"
#include "ConsoleConfig.h"
#include "Board.h"
#include "Cell.h"

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


//private:
//    static DrawOptions s_Instance;

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
    console.PrintIntro();

    Board board(60, 30);

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
    auto& Ruleset = C;

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
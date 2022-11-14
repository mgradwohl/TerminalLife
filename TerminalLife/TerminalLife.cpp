// TerminalLife.cpp
#include "pch.h"
#include "ConsoleConfig.h"
#include "Board.h"
#include "Cell.h"
#include "hud.h"

int main()
{
    ConsoleConfig console;
    HUD::Get().PrintIntro();
    console.DrawBegin();
    Board board(console.Width() / 2, console.Height() - 10);

    // Randomly fill  spots for n 'generations'
    int n = board.Width() * board.Height() / 4;
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

    // simulation loop
    while (true)
    {
        console.SetPositionHome();
        if (!HUD::Get().Update(board))
            break;

        // print the board to the console AND flush the stream
        console.SetPositionBoard();
        board.PrintBoard();

        HUD::Get().HandleIncremental();

        // TODO this is bad
        Cell::SetOldAge(HUD::Get().OldAge());

        board.UpdateBoard(Ruleset);

        // this will show the user the pending changes to the board (born, dying, etc.)
        if (HUD::Get().Fate())
        {
            console.SetPositionHome();
            HUD::Get().Update(board);

            // print the board with Fates to the console AND flush the stream
            console.SetPositionBoard();
            board.PrintBoard();

            HUD::Get().HandleIncremental();
        }

        // this applies the changes that were determined by the ruleset called by Board::UpdateBoard();
        board.NextGeneration();
    }

    console.Clear();
    std::cout << "\x1b[mThanks for the simulation!" << std::endl;

    // console dtor will restore the console
}
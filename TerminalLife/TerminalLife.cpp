// TerminalLife.cpp
#include "pch.h"
#include "ConsoleConfig.h"
#include "Board.h"
#include "Cell.h"
#include "hud.h"

int main()
{
    ConsoleConfig console;
    HUD::PrintIntro();
    console.DrawBegin();
    Board board(console.Width() / 2, console.Height() - 10);

    // Randomly fill  spots for n 'generations'
    int n = board.Width() * board.Height() / 4;
    board.RandomizeBoard(n);
    
    // Rulesets
    auto C = std::bind_front(&Board::ConwayRules, &board);
    auto D = std::bind_front(&Board::DayAndNightRules, &board);
    auto S = std::bind_front(&Board::SeedsRules, &board);
    auto B = std::bind_front(&Board::BriansBrainRules, &board);
    auto H = std::bind_front(&Board::HighlifeRules, &board);
    auto L = std::bind_front(&Board::LifeWithoutDeathRules, &board);

    // pick your Ruleset here
    auto const& Ruleset = C;

    // simulation loop
    while (true)
    {
        console.SetPositionHome();
        if (!HUD::Update(board))
            break;

        // print the board to the console AND flush the stream
        console.SetPositionBoard();
        board.PrintBoard();

        HUD::HandleIncremental();

        // TODO this is bad
        Cell::SetOldAge(HUD::OldAge());

        board.UpdateBoard(Ruleset);

        // this will show the user the pending changes to the board (born, dying, etc.)
        if (HUD::Fate())
        {
            console.SetPositionHome();
            HUD::Update(board);

            // print the board with Fates to the console AND flush the stream
            console.SetPositionBoard();
            board.PrintBoard();

            HUD::HandleIncremental();
        }

        // this applies the changes that were determined by the ruleset called by Board::UpdateBoard();
        board.NextGeneration();
    }

    console.Clear();
    std::cout << "\x1b[mThanks for the simulation!" << std::endl;

    // console dtor will restore the console
}
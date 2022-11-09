﻿// for visualization purposes (0,0) is the top left.
// as x increases move right, as y increases move down
#include "pch.h"
#include "Board.h"

Board::Board(int width, int height)
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

int Board::CountLiveNeighbors(Cell& cell)
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

int Board::CountLiveNotDyingNeighbors(Cell& cell)
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
void Board::NextGeneration()
{
	_generation++;
	Cell::ResetCounts();

	for (int i = 0; i < _size; i++)
	{
		_board[i].NextGeneration();
	}
}

void Board::RandomizeBoard(int n)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> xdis(0, _width - 1);
	std::uniform_int_distribution<> ydis(0, _height - 1);
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

//// This form does not work: void UpdateBoard(std::function<void(Cell& cell)>& F)
//// but using auto is magic
//void Board::UpdateBoard(auto F)
//{
//	for (int y = 0; y < Height(); y++)
//	{
//		for (int x = 0; x < Width(); x++)
//		{
//			Cell& cc = GetCell(x, y);
//			CountLiveNeighbors(cc);
//			F(cc);
//			cc.KillOldCell();
//		}
//	}
//}

void Board::ConwayRules(Cell& cell) const
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

void Board::DayAndNightRules(Cell& cell) const
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
	else if (cell.IsDead() && (count == 3 || count >= 6))
	{
		cell.SetState(Cell::State::Born);
	}
	else if (cell.IsAlive())
	{
		cell.SetState(Cell::State::Dying);
	}
}

void Board::LifeWithoutDeathRules(Cell& cell) const
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

void Board::HighlifeRules(Cell& cell) const
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

void Board::SeedsRules(Cell& cell) const
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

void Board::BriansBrainRules(Cell& cell) const
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

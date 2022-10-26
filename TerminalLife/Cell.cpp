#include "pch.h"
#include "cell.h"

void Cell::SetState(State state)
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

bool Cell::IsAlive() const
{
	if (_state == Cell::State::Live || _state == Cell::State::Dying || _state == Cell::State::Old)
	{
		return true;
	}
	return false;
}

bool Cell::IsAliveNotDying() const
{
	if (_state == Cell::State::Live)
	{
		return true;
	}
	return false;
}

bool Cell::IsDead() const
{
	if (_state == Cell::State::Dead || _state == Cell::State::Born)
	{
		return true;
	}
	return false;
}

const char* Cell::GetStateString() const
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

const char8_t* Cell::GetEmojiStateString() const
{
	static char8_t cDead[8] = u8"🖤\0";
	static char8_t cLive[8] = u8"😁\0";
	static char8_t cBorn[8] = u8"💕\0";
	static char8_t cOld[8] = u8"🤡\0";
	static char8_t cDying[8] = u8"🤢\0";
	static char8_t cUnknown[8] = u8"⁉️\0";

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

void Cell::NextGeneration()
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

void Cell::KillOldCell()
{
	// we only enforce this rule if age > 0
	if (Cell::OldAge > 0)
	{
		if (_age == Cell::OldAge - 2)
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

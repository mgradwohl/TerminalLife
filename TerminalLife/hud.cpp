﻿#include "pch.h"
#include "hud.h"
#include "Board.h"

bool HUD::CheckKeyStateImpl ()
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

void HUD::PrintIntroImpl() const 
{
	std::cout << "\x1b[mWelcome to TerminalLife\r\n\r\nResize your console to get the biggest simulation\r\n";
	std::cout << "\x1b[mENTER to start\r\nSPACE to pause/unpause\r\nESC to quit\r\n[+] and [-] to change speed\r\n[S] to toggle the HUD\r\n[F] to show cell fates\r\n[I] to toggle incremental vs. continuous simulation" << std::endl;
	std::cin.get();
}

bool HUD::UpdateImpl(const Board& board) const
{
	if (!HUD::CheckKeyState())
		return false;

	if (HUD::Score())
	{
		std::cout << "\x1b[mGeneration " << board.Generation() << ". Sleep: " << HUD::Delay() << ". Life Span: " << HUD::OldAge() << ". Alive: " << Cell::GetLiveCount() << ". Dead: " << Cell::GetDeadCount() << ". Born: " << Cell::GetBornCount() << ". Dying: " << Cell::GetDyingCount() << ". OldAge: " << Cell::GetOldCount() << ".\x1b[0K\n";
	}
	else std::cout << "\x1b[2K\n";

	if (HUD::Incremental())
	{
		std::cout << "\x1b[mHit SPACE for next screen, [I] to continuously update\n\n";
	}
	else std::cout << "\x1b[2K\n";

	return true;
}


void HUD::HandleIncrementalImpl() const
{
	if (HUD::Incremental())
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
	else Sleep(HUD::Delay());
}

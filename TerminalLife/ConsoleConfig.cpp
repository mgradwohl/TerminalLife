#include "pch.h"
#include "ConsoleConfig.h"

ConsoleConfig::ConsoleConfig()
{
	// make sure Windows Console and C++ runtime are set for utf8
	auto UTF8 = std::locale("en_US.UTF-8");
	std::locale::global(UTF8);
	std::cout.imbue(UTF8);
	setlocale(LC_ALL, "en_us.utf8");
	SetConsoleOutputCP(CP_UTF8);

	// don't sync the C++ streams to the C streams, for performance
	std::ios::sync_with_stdio(false);
}

ConsoleConfig::~ConsoleConfig()
{
	SetConsoleMode(_hOut, _dwOriginalOutMode);
}

bool ConsoleConfig::ConsoleInit()
{
	// Set output mode to handle virtual terminal sequences
	_hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (_hOut == INVALID_HANDLE_VALUE)
	{
		std::cout << "TerminalLife: Error setting console preferences. 0x01" << std::endl;
		return false;
	}

	if (!GetConsoleMode(_hOut, &_dwOriginalOutMode))
	{
		std::cout << "TerminalLife: Error setting console preferences. 0x02" << std::endl;
		return false;
	}

	DWORD dwRequestedOutModes = ENABLE_VIRTUAL_TERMINAL_PROCESSING /*| DISABLE_NEWLINE_AUTO_RETURN*/;

	DWORD dwOutMode = _dwOriginalOutMode | dwRequestedOutModes;
	if (!SetConsoleMode(_hOut, dwOutMode))
	{
		// we failed to set both modes, try to step down mode gracefully.
		dwRequestedOutModes = ENABLE_VIRTUAL_TERMINAL_PROCESSING;
		dwOutMode = _dwOriginalOutMode | dwRequestedOutModes;
		if (!SetConsoleMode(_hOut, dwOutMode))
		{
			std::cout << "TerminalLife: Error setting console preferences. 0x03" << std::endl;
			return false;
		}
	}
	return true;
}

void ConsoleConfig::PrintIntro()
{
	system("CLS");
	std::cout << "\x1b[mWelcome to TerminalLife, resize your console to get the biggest simulation, then press [ENTER]" << std::endl;
	std::cout << "\x1b[mHit ENTER to start, SPACE to pause/unpause, ESC to quit, [+] and [-] to change speed, [S] to show details, [F] to show cell fates, and [I] to toggle incremental vs. continuous simulation" << std::endl;
	std::cin.get();

	GetConsoleScreenBufferInfo(_hOut, &_csbi);
}

void ConsoleConfig::DrawBegin()
{
	// turn off the cursor
	std::cout << "\x1b[?25l" << std::endl;

	//untie cin and cout, since we won't use cin anymore and this improves performance
	std::cin.tie(nullptr);

	Clear();
}
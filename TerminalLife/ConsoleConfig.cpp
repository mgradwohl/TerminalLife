#include "pch.h"
#include "ConsoleConfig.h"

ConsoleConfig::ConsoleConfig()
{
	// Set output mode to handle virtual terminal sequences
	_hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (_hOut == INVALID_HANDLE_VALUE)
	{
		std::cout << "TerminalLife: Error setting console preferences. 0x01" << std::endl;
		exit(-1);
	}

	if (!GetConsoleMode(_hOut, &_dwOriginalOutMode))
	{
		std::cout << "TerminalLife: Error setting console preferences. 0x02" << std::endl;
		exit(-1);
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
			exit(-1);
		}
	}

	// don't sync the C++ streams to the C streams, for performance
	std::ios::sync_with_stdio(false);

	// make sure Windows Console and C++ runtime are set for utf8
	auto UTF8 = std::locale("en_US.UTF-8");
	std::locale::global(UTF8);
	std::cout.imbue(UTF8);
	setlocale(LC_ALL, "en_us.utf8");
	SetConsoleOutputCP(CP_UTF8);

	if (GetConsoleOutputCP() != 65001)
	{
		std::cout << "TerminalLife: Error setting console preferences. 0x04" << std::endl;
		exit(-1);
	}
}

ConsoleConfig::~ConsoleConfig()
{
	SetConsoleMode(_hOut, _dwOriginalOutMode);
}

void ConsoleConfig::DrawBegin()
{
	GetConsoleScreenBufferInfo(_hOut, &_csbi);

	// turn off the cursor
	std::cout << "\x1b[?25l" << std::endl;

	//untie cin and cout, since we won't use cin anymore and this improves performance
	std::cin.tie(nullptr);

	Clear();
}
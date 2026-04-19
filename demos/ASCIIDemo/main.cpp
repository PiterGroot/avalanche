#include <iostream>
#include <string>
#include <windows.h>
#include <vector>
#include <conio.h>
#include <chrono>

#include <DemoCore/include/avalanche_initializer.hpp>

#include <DemoCore/include/cell-behaviours/sand.hpp>
#include <DemoCore/include/cell-behaviours/fire.hpp>
#include <DemoCore/include/cell-behaviours/water.hpp>
#include <DemoCore/include/cell-behaviours/smoke.hpp>
#include <DemoCore/include/cell-behaviours/lava.hpp>
#include <DemoCore/include/cell-behaviours/gun_powder.hpp>
#include <DemoCore/include/cell-behaviours/plant.hpp>

#include "console_cursor.hpp"

int currentCellID = 2;
ConsoleCursor::CursorPosition ConsoleCursor::cursorPosition = { 20,20 };
const char* ConsoleCursor::currentCellTextInfo = "-SAND";

// Cursor movement state
static float cursorMoveTimer = 0.0f;
static const float cursorMoveDelay = 0.01f; // 50ms delay between moves (adjustable)

static char MapCellIDToChar(int cellID)
{
	switch (cellID)
	{
		case 0:  return ' '; // empty
		case 1:  return '#'; // wall
		case 2:  return '.'; // sand
		case 3:  return '~'; // water
		case 4:  return 'x'; // fire
		case 5:  return '+'; // wood
		case 6:  return '^'; // smoke
		case 8: return '!'; // gunpowder
		case 9: return '&'; // plant
		case 10: return '*'; // spark
		case 13: return '$'; // quantum
		default: return '?'; // unknown
	}
}

static void HandleInput(const HANDLE& consoleHandle, avl::World& world, float deltaTime)
{
	if (GetAsyncKeyState('Q') & 0x8000)
		exit(0);

	if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
	{
		if (GetAsyncKeyState(VK_SPACE) & 0x8000)
		{
			ConsoleCursor::CursorPosition pos = ConsoleCursor::cursorPosition;
			world.plot_cell(pos.cursorX, pos.cursorY, currentCellID);
		}
	}
	else if (GetAsyncKeyState(VK_SPACE) & 0x8000)
	{
		ConsoleCursor::CursorPosition pos = ConsoleCursor::cursorPosition;
		world.plot_circle(pos.cursorX, pos.cursorY, 2, currentCellID, 0.0f);
	}

	if (GetAsyncKeyState(VK_BACK) & 0x8000)
	{
		ConsoleCursor::CursorPosition pos = ConsoleCursor::cursorPosition;
		world.plot_circle(pos.cursorX, pos.cursorY, 2, 0, 0.0f);
	}

	static bool numKeyPressed = false;
	bool anyNumKeyDown = false;

	if (GetAsyncKeyState('1') & 0x8000) { currentCellID = 1; ConsoleCursor::currentCellTextInfo = "-WALL"; anyNumKeyDown = true; }
	else if (GetAsyncKeyState('2') & 0x8000) { currentCellID = 2; ConsoleCursor::currentCellTextInfo = "-SAND"; anyNumKeyDown = true; }
	else if (GetAsyncKeyState('3') & 0x8000) { currentCellID = 3; ConsoleCursor::currentCellTextInfo = "-WATER"; anyNumKeyDown = true; }
	else if (GetAsyncKeyState('4') & 0x8000) { currentCellID = 4; ConsoleCursor::currentCellTextInfo = "-FIRE"; anyNumKeyDown = true; }
	else if (GetAsyncKeyState('5') & 0x8000) { currentCellID = 5; ConsoleCursor::currentCellTextInfo = "-WOOD"; anyNumKeyDown = true; }
	else if (GetAsyncKeyState('6') & 0x8000) { currentCellID = 8; ConsoleCursor::currentCellTextInfo = "-GUN_POWDER"; anyNumKeyDown = true; }
	else if (GetAsyncKeyState('7') & 0x8000) { currentCellID = 9; ConsoleCursor::currentCellTextInfo = "-PLANT"; anyNumKeyDown = true; }
	else if (GetAsyncKeyState('8') & 0x8000) { currentCellID = 10; ConsoleCursor::currentCellTextInfo = "-SPARK"; anyNumKeyDown = true; }
	else if (GetAsyncKeyState('9') & 0x8000) { currentCellID = 13; ConsoleCursor::currentCellTextInfo = "-QUANTUM"; anyNumKeyDown = true; }

	numKeyPressed = anyNumKeyDown;

	// Accumulate delta time for cursor movement
	cursorMoveTimer += deltaTime;

	// Check if enough time has passed to move cursor
	if (cursorMoveTimer >= cursorMoveDelay)
	{
		int deltaX = 0, deltaY = 0;

		if (GetAsyncKeyState('W') & 0x8000) deltaY -= 1;
		if (GetAsyncKeyState('S') & 0x8000) deltaY += 1;
		if (GetAsyncKeyState('A') & 0x8000) deltaX -= 1;
		if (GetAsyncKeyState('D') & 0x8000) deltaX += 1;

		if (deltaX != 0 || deltaY != 0)
		{
			ConsoleCursor::cursorPosition.AddPosition(deltaX, deltaY);
			cursorMoveTimer = 0.0f; // Reset timer after moving
		}
		else
		{
			// If no movement keys pressed, reset timer
			cursorMoveTimer = 0.0f;
		}
	}
}

static void SetupConsole(int width, int height)
{
	HWND consoleWindow = GetConsoleWindow();
	if (consoleWindow == NULL)
	{
		std::cerr << "Error: Could not get console window handle." << std::endl;
		return;
	}

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	CONSOLE_CURSOR_INFO cursorInfo{};
	cursorInfo.bVisible = FALSE;
	SetConsoleCursorInfo(hConsole, &cursorInfo);

	COORD coord = { (SHORT)width, (SHORT)height };
	if (!SetConsoleScreenBufferSize(hConsole, coord))
	{
		std::cerr << "SetConsoleScreenBufferSize failed: " << GetLastError() << std::endl;
		return;
	}

	SMALL_RECT windowSize = { 0, 0, 119, 49 };
	SetConsoleWindowInfo(hConsole, TRUE, &windowSize);

	COORD cursorPos = { 0, 0 };
	SetConsoleCursorPosition(hConsole, cursorPos);

	FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
}

static void DrawCustomCursor(std::vector<CHAR_INFO>& buffer, int bufferWidth)
{
	int cursorHandleIndex = ConsoleCursor::cursorPosition.cursorX + (ConsoleCursor::cursorPosition.cursorY - 1) * bufferWidth;
	int cursorHeadIndex = ConsoleCursor::cursorPosition.cursorX + ConsoleCursor::cursorPosition.cursorY * bufferWidth;

	buffer[cursorHandleIndex].Char.AsciiChar = '|';
	buffer[cursorHandleIndex].Attributes = FOREGROUND_RED;

	buffer[cursorHeadIndex].Char.AsciiChar = 'v';
	buffer[cursorHeadIndex].Attributes = FOREGROUND_RED;

	int size = strlen(ConsoleCursor::currentCellTextInfo);

	for (size_t i = 0; i < size; i++)
	{
		int cellNameIndex = (ConsoleCursor::cursorPosition.cursorX + i + 1) + (ConsoleCursor::cursorPosition.cursorY - 1) * bufferWidth;
		buffer[cellNameIndex].Char.AsciiChar = ConsoleCursor::currentCellTextInfo[i];
		buffer[cellNameIndex].Attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
	}
}

static void SetConsoleBufferCell(int x, int y, int cellID, std::vector<CHAR_INFO>& buffer, int bufferWidth)
{
	int index = x + y * bufferWidth;

	buffer[index].Char.AsciiChar = MapCellIDToChar(cellID);
	buffer[index].Attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;

	DrawCustomCursor(buffer, bufferWidth);
}

static void WriteBufferToConsole(const std::vector<CHAR_INFO>& buffer, const avl::SimulationSector* sector)
{
	COORD bufferSize = { sector->width, sector->height };
	COORD bufferCoord = { 0, 0 };

	SMALL_RECT writeRegion = { 0, 0, sector->width - 1, sector->height - 1 };
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

	WriteConsoleOutput(consoleHandle, buffer.data(), bufferSize, bufferCoord, &writeRegion);
}

int main()
{
	SetupConsole(500, 500);
	static AvalancheInitializer avalancheInitializer = AvalancheInitializer();

	for (size_t i = 0; i < 200; i++)
	{
		avalancheInitializer.world->plot_cell(0 + i, 1, 1);
		avalancheInitializer.world->plot_cell(0 + i, 2, 1);
		avalancheInitializer.world->plot_cell(0 + i, 50, 1);
	}

	avalancheInitializer.world->plot_cell(0, 3, 1);
	avalancheInitializer.world->plot_cell(199, 3, 1);

	for (size_t i = 0; i < 10; i++)
	{
		avalancheInitializer.world->plot_cell(0, 50 - i, 1);
		avalancheInitializer.world->plot_cell(200, 50 - i, 1);
	}

	avalancheInitializer.world->plot_circle(60, 30, 8, 5, 0.0f);
	avalancheInitializer.world->plot_cell(60, 30, 4);

	bool windowShouldClose = false;
	auto startTime = std::chrono::high_resolution_clock::now();
	float fixedInterval = 50.0f;
	float fixedStepTime = 1.0f / fixedInterval;

	std::vector<CHAR_INFO> consoleBuffer(500 * 500);
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

	while (!windowShouldClose)
	{
		auto currentTime = std::chrono::high_resolution_clock::now();
		float deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

		startTime = currentTime;
		avalancheInitializer.world->step_world_without_callback(deltaTime, fixedStepTime);

		static avl::SimulationSector* originSector = avalancheInitializer.world->get_sector_direct(0);

		for (int i = 0; i < originSector->simulationSize; ++i)
		{
			consoleBuffer[i].Char.AsciiChar = ' ';
			consoleBuffer[i].Attributes =
				FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
		}

		for (int i = 0; i < originSector->simulationSize; i++)
		{
			int xPosition = i % originSector->width;
			int yPosition = i / originSector->height;

			int targetCellType = avalancheInitializer.world->get_cell_id_unsafe(xPosition, yPosition);

			if (targetCellType > 0)
				SetConsoleBufferCell(xPosition, yPosition, targetCellType, consoleBuffer, originSector->width);
		}

		WriteBufferToConsole(consoleBuffer, originSector);
		HandleInput(consoleHandle, *avalancheInitializer.world, deltaTime);
	}
}
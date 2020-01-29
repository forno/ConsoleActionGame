#include "render.h"

#include <Windows.h>

render::render()
{
  COORD topLeft{ 0, 0 };
  HANDLE console{ GetStdHandle(STD_OUTPUT_HANDLE) };
  CONSOLE_SCREEN_BUFFER_INFO screen;
  GetConsoleScreenBufferInfo(console, &screen);
  DWORD written;
  FillConsoleOutputCharacterA(
    console, ' ', screen.dwSize.X * screen.dwSize.Y, topLeft, &written
  );
  FillConsoleOutputAttribute(
    console, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE,
    screen.dwSize.X * screen.dwSize.Y, topLeft, &written
  );
  SetConsoleCursorPosition(console, topLeft);
}
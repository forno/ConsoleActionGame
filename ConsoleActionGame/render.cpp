#include "render.h"

#include <stdexcept>

#include <Windows.h>

render::render(std::mutex& m_)
  : m {m_}
{
  constexpr COORD top_left{ 0, 0 };
  HANDLE output_handle{ GetStdHandle(STD_OUTPUT_HANDLE) };
  if (output_handle == INVALID_HANDLE_VALUE)
    throw std::runtime_error{ "render: Fail GetStdHandle" };
  CONSOLE_SCREEN_BUFFER_INFO screen;
  if (!GetConsoleScreenBufferInfo(output_handle, &screen))
    throw std::runtime_error{ "render: Fail GetConsoleScreenBufferInfo" };
  DWORD write_count;
  if (!FillConsoleOutputCharacter(output_handle, ' ', screen.dwSize.X * screen.dwSize.Y, top_left, &write_count))
    throw std::runtime_error{ "render: Fail FillConsoleOutputCharacter" };
  if (!FillConsoleOutputAttribute(output_handle, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE, screen.dwSize.X * screen.dwSize.Y, top_left, &write_count))
    throw std::runtime_error{ "render: Fail FillConsoleOutputAttribute" };
  SetConsoleCursorPosition(output_handle, top_left);
}
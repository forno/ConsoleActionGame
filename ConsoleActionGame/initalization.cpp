#include <windows.h>

HANDLE input_handle;
static DWORD console_old_mode;

[[noreturn]]
void myexit()
{
	SetConsoleMode(input_handle, console_old_mode);
	ExitProcess(0);
}

bool initalize()
{
	static bool is_init{ false };
	if (is_init)
		return true;

	if ((input_handle = GetStdHandle(STD_INPUT_HANDLE)) == INVALID_HANDLE_VALUE)
		return false;
	if (!GetConsoleMode(input_handle, &console_old_mode))
		return false;
	if (!SetConsoleMode(input_handle, ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS))
		return false;

	is_init = true;
	return true;
}

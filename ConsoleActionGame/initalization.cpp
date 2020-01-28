#include "initalization.h"

#include <stdexcept>
#include <windows.h>

#ifdef __cplusplus

class console_initalizaiton::impl
{
public:
  impl(HANDLE& input_handle)
    : handle{ input_handle }
  {
    if (!GetConsoleMode(input_handle, &console_old_mode))
      throw std::runtime_error{ "console_initalizaiton: Fail GetConsoleMode" };
    if (!SetConsoleMode(input_handle, ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS))
      throw std::runtime_error{ "console_initalizaiton: Fail SetConsoleMode" };
  }

  ~impl() noexcept
  {
    SetConsoleMode(handle, console_old_mode);
  }

private:
  HANDLE& handle;
  DWORD console_old_mode;
};

console_initalizaiton::console_initalizaiton(HANDLE& input_handle)
  : pimpl{ new impl{ input_handle } } {}

console_initalizaiton::~console_initalizaiton() { delete pimpl; }

#else

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

#endif

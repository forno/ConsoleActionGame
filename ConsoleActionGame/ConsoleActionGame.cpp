#include <variant>
#include <Windows.h>

#include "input_manager.h"
#include "status.h"

int main(void)
{
  HANDLE input_handle;
  if ((input_handle = GetStdHandle(STD_INPUT_HANDLE)) == INVALID_HANDLE_VALUE)
    return;

  input_manager im{ input_handle };

  status s{ state::title{} };

  while (true) {
  }
  return 0;
}

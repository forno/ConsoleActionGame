#include <Windows.h>

#include <cstdlib>

#include "input_manager.h"
#include "status.h"
#include "updater.h"
#include "render.h"

int main(void)
{
  HANDLE input_handle{ GetStdHandle(STD_INPUT_HANDLE) };
  if (input_handle == INVALID_HANDLE_VALUE)
    return EXIT_SUCCESS;

  input_manager im{ input_handle };
  status s{ state::title{ im } };

  while (true) {
    std::visit(render{}, s);
    im.update();
    s = std::visit(updater{ im }, s);
  }
  return EXIT_SUCCESS;
}

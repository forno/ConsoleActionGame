#include "updater.h"
#include "render.h"

#include <iostream>

state::gaming::gaming(input_manager& im)
{
  im.set_native(true);
}

status updater::operator()(state::gaming&)
{
  return state::result{ im };
}

void render::operator()(state::gaming&)
{
  std::cout << "gaming:\n" <<
               "> " << std::flush;
}

#include "updater.h"
#include "render.h"

#include <iostream>

state::result::result(input_manager& im)
  : score{}
{
  im.set_native(false);
}

status updater::operator()(state::result&)
{
  return state::title{ im };
}

void render::operator()(state::result&)
{
  std::cout << "result:\n" <<
               "> " << std::flush;
}

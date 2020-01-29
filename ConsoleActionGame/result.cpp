#include "updater.h"
#include "render.h"

#include <iostream>

state::result::result(input_manager& im, unsigned int score_)
  : score{ score_ }
{
  im.set_native(false);
}

status updater::operator()(state::result&)
{
  return state::title{ im };
}

void render::operator()(state::result& v)
{
  std::cout << "result: " << v.score << '\n' <<
               "> " << std::flush;
}

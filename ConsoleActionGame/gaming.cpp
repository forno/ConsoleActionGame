#include "updater.h"
#include "render.h"

#include <iostream>
#include <chrono>

class state::gaming::impl
{

private:
  std::chrono::steady_clock::time_point next_time{ std::chrono::steady_clock::time_point::max() };
};

state::gaming::gaming(input_manager& im)
  : pimpl{ new impl{} }
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

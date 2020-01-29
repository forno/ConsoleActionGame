#include "updater.h"
#include "render.h"

#include <iostream>

state::title::title(input_manager& im)
{
  im.set_native(false);
}

status updater::operator()(state::title&)
{
  const auto& input{ im.getline() };
  if (input.empty())
    return state::title{ im };
  switch (input.front()) {
  case '1': [[fallthrough]];
  case 'c': [[fallthrough]];
  case 'g':
    return state::gaming{ im };
  case '2': [[fallthrough]];
  case 'e': [[fallthrough]];
  case 'f': [[fallthrough]];
  case 'q':
    return state::finish{ im };
  default:
    return state::title{ im };
  }
  return state::title{ im };
}

void render::operator()(state::title&)
{
  std::cout <<
    "This is title:\n" <<
    "You can choice continue or die.\n" <<
    "continue: 1, quit: 2\n" <<
    "> " << std::flush;
}

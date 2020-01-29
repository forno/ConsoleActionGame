#include "updater.h"
#include "render.h"

#include <iostream>
#include <limits>

state::title::title(input_manager& im)
{
  im.set_native(false);
}

status updater::operator()(state::title&)
{
  const auto& input{ im.getline() };
  if (!input.empty() && input.front() == '1') {
    im.set_native(true);
    return state::gaming{ im };
  }
  return state::title{ im };
}

void render::operator()(state::title&)
{
  std::cout <<
    "This is title:\n" <<
    "You can choice continue or die.\n" <<
    "continue: 1, quit: 2\n" <<
    "> ";
}

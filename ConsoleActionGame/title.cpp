#include "updater.h"
#include "render.h"

#include <iostream>

state::title::title(input_manager& im)
{
  im.set_native(false);
}

bool state::title::operator==(const title&) const noexcept { return true; }

status updater::operator()(const state::title& v)
{
  const auto& input{ im.getline() };
  if (input.empty()) {
    return v;
  }
  switch (input.front()) {
  case '1': [[fallthrough]];
  case 'c': [[fallthrough]];
  case 'g':
    return state::gaming{ im };
  case '2': [[fallthrough]];
  case 'e': [[fallthrough]];
  case 'f': [[fallthrough]];
  case 'q':
    return std::monostate{};
  default:
    return v;
  }
  return v;
}

void render::operator()(const state::title&)
{
  std::cout <<
    "This is title:\n" <<
    "You can choice continue or die.\n" <<
    "continue: 1, quit: 2\n" <<
    "> " << std::flush;
}

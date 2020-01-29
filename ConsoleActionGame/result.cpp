#include "updater.h"
#include "render.h"

#include <iostream>

state::result::result(input_manager& im, unsigned int score_)
  : score{ score_ }
{
  im.set_native(false);
}

bool state::result::operator==(const result& rhs) const noexcept { return score == rhs.score; }

status updater::operator()(const state::result& v)
{
  const auto& input{ im.getline() };
  if (input.empty()) {
    return v;
  }
  switch (input.front()) {
  case '1': [[fallthrough]]; // 1
  case 'c': [[fallthrough]]; // continue
  case 'e': [[fallthrough]]; // end
  case 'f': [[fallthrough]]; // finish
  case 'g': [[fallthrough]]; // gaming
  case 'q': [[fallthrough]]; // quit
  case 't':                  // title
    return state::title{ im };
  default:
    return v;
  }
  return v;
}

void render::operator()(const state::result& v)
{
  static bool init{ false };
  std::cout << "result: " << v.score << '\n' <<
               "Enter 'q' to go to title > " << std::flush;
}

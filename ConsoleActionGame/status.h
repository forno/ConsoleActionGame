#pragma once

#include <variant>

namespace state
{
  struct title {};
  struct gaming { int atk; int life; };
  struct result { int score; };
  struct finish {};
}

using status = std::variant<state::title, state::gaming, state::result, state::finish>;

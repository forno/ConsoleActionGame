#pragma once

namespace state
{
  struct title {};
  struct gaming {};
  struct result {};
  struct finish {};
}

struct data
{
  int score; // TODO: modify for share datas
};

using status = std::variant<state::title, state::gaming, state::result, state::finish>;

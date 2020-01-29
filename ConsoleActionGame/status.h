#pragma once

#include <variant>

#include "input_manager.h"

namespace state
{

struct title {
  title(input_manager& im);
};

struct gaming {
  struct impl;
  impl* pimpl;

  gaming(input_manager& im);
};
struct result {
  unsigned int score;

  result(input_manager& im, unsigned int score);
};
struct finish {
  finish(input_manager& im);
};

}

using status = std::variant<state::title, state::gaming, state::result, state::finish>;

bool is_finish(const status& v) noexcept;

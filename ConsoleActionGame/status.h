#pragma once

#include <variant>

#include "input_manager.h"

namespace state
{
  struct title {
    title(input_manager& im);
  };
  struct gaming {
    gaming(input_manager& im);
  };
  struct result {
    result(input_manager& im);

    int score;
  };
  struct finish {
    finish(input_manager& im);
  };
}

using status = std::variant<state::title, state::gaming, state::result, state::finish>;

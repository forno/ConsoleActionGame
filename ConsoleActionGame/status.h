#pragma once

#include <variant>

#include "input_manager.h"

namespace state
{

struct title {
  title(input_manager& im);
  bool operator==(const title& rhs) const noexcept;
};

struct gaming {
  struct impl;
  impl* pimpl;

  gaming(input_manager& im);
  ~gaming() noexcept;
  gaming(const gaming& g);
  gaming& operator=(const gaming& g);
  gaming(gaming&& g) noexcept;
  gaming& operator=(gaming&& g) noexcept;
  bool operator==(const gaming& g) const noexcept;
};
struct result {
  unsigned int score;

  result(input_manager& im, unsigned int score);
  bool operator==(const result& rhs) const noexcept;
};

}

using status = std::variant<state::title, state::gaming, state::result, std::monostate>;

bool is_finish(const status& v) noexcept;

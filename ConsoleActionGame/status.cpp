#include "status.h"

bool is_finish(const status& v) noexcept
{
  return std::holds_alternative<state::finish>(v);
}
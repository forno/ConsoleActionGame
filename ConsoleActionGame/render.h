#pragma once

#include "status.h"

struct render
{
  render();

  void operator()(state::title&);
  void operator()(state::gaming&);
  void operator()(state::result&);
  [[noreturn]]
  void operator()(state::finish&);
};

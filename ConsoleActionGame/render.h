#pragma once

#include <mutex>

#include "status.h"

struct render
{
  render();

  void operator()(const state::title&);
  void operator()(const state::gaming&);
  void operator()(const state::result&);
  [[noreturn]]
  void operator()(const state::finish&);
};

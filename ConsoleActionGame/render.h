#pragma once

#include <mutex>

#include "status.h"

struct render
{
  std::mutex& m;
  render(std::mutex& m);

  void operator()(const state::title&);
  void operator()(const state::gaming&);
  void operator()(const state::result&);
  [[noreturn]]
  void operator()(const state::finish&);
};

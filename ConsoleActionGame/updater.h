#pragma once

#include <mutex>

#include "input_manager.h"
#include "status.h"

struct updater
{
  input_manager& im;

  status operator()(const state::title&);
  status operator()(const state::gaming&);
  status operator()(const state::result&);
  status operator()(const std::monostate&) { std::exit(0); }
};

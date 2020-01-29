#pragma once

#include "input_manager.h"
#include "status.h"

struct updater
{
  input_manager& im;

  status operator()(state::title&);
  status operator()(state::gaming&);
  status operator()(state::result&);
  [[noreturn]]
  status operator()(state::finish&);
};

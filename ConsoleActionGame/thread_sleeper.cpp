#pragma once

#include "thread_sleeper.h"

thread_sleeper::~thread_sleeper() noexcept
{
  std::this_thread::sleep_until(awake_time_point);
}

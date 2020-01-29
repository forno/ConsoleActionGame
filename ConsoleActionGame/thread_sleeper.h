#pragma once

#include <chrono>
#include <thread>

class thread_sleeper
{
public:
  template<typename... Args>
  thread_sleeper(std::chrono::duration<Args...> duration)
  : awake_time_point{ std::chrono::steady_clock::now() + duration }
  { }

  ~thread_sleeper() noexcept;

private:
  std::chrono::steady_clock::time_point awake_time_point;
};

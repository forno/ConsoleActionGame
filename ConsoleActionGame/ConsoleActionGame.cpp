#include <Windows.h>

#include <atomic>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <mutex>
#include <thread>

#include "input_manager.h"
#include "render.h"
#include "status.h"
#include "thread_sleeper.h"
#include "updater.h"

int main(void)
{
  HANDLE input_handle{ GetStdHandle(STD_INPUT_HANDLE) };
  if (input_handle == INVALID_HANDLE_VALUE)
    return EXIT_SUCCESS;

  input_manager im{ input_handle };
  status s{ state::title{ im } };
  std::mutex m;
  std::atomic_bool is_termination{ false };
  std::atomic_bool is_changed{ true };

  std::thread render_thread{ [&]() {
    while (!is_termination) {
      using namespace std::literals::chrono_literals;
      thread_sleeper ts{ 1000ms / 60 };
      if (is_changed) {
        is_changed = false;
        std::lock_guard lg{ m };
        std::visit(render{}, s);
      }
    }
  } };

  while (!is_finish(s)) {
    using namespace std::literals::chrono_literals;
    thread_sleeper ts{ 1000ms / 60 };
    im.update();
    auto next_state{ std::visit(updater{ im }, s) };
    if (!(s == next_state))
      is_changed = true;
    std::lock_guard lg{ m };
    s = std::move(next_state);
  }
  is_termination = true;
  render_thread.join();
  return EXIT_SUCCESS;
}

#include <Windows.h>

#include <atomic>
#include <chrono>
#include <cstdlib>
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
  std::atomic_bool f{ true };

  std::thread render_thread{ [&]() {
    while (f) {
      using namespace std::literals::chrono_literals;
      const thread_sleeper ts{ 1000ms / 5 };
      {
        std::lock_guard lg{ m };
        std::visit(render{}, s);
      }
    }
  } };
  std::thread input_thread{ [&]() {
    while (f) {
      using namespace std::literals::chrono_literals;
      const thread_sleeper ts{ 1000ms / 60 };
      im.update();
    }
  } };

  while (!is_finish(s)) {
    auto s_tmp{ std::visit(updater{ im }, s) };
    std::lock_guard lg{ m };
    s = std::move(s_tmp);
  }
  f = false;
  input_thread.join();
  render_thread.join();
  return EXIT_SUCCESS;
}

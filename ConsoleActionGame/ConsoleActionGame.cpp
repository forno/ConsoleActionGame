#include <Windows.h>

#include <atomic>
#include <cstdlib>
#include <mutex>
#include <thread>

#include "input_manager.h"
#include "status.h"
#include "updater.h"
#include "render.h"

int main(void)
{
  HANDLE input_handle{ GetStdHandle(STD_INPUT_HANDLE) };
  if (input_handle == INVALID_HANDLE_VALUE)
    return EXIT_SUCCESS;

  input_manager im{ input_handle };
  status s{ state::title{ im } };
  std::mutex m;
  std::atomic_bool f{ true };

  std::thread t{ [&]() {while (f) std::visit(render{ m }, s); } };

  while (!is_finish(s)) {
    im.update();
    auto s_tmp = std::visit(updater{ im }, s);
    std::lock_guard lg{ m };
    s = std::move(s_tmp);
  }
  f = false;
  t.join();
  return EXIT_SUCCESS;
}

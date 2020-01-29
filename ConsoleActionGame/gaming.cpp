#include "updater.h"
#include "render.h"

#include <chrono>
#include <iostream>
#include <variant>

namespace
{
namespace game
{

struct count_down
{
  std::chrono::steady_clock::time_point time_limit;
};
struct enter_mash
{
  std::chrono::steady_clock::time_point time_limit;
  unsigned int count{ 0 };
};
struct finish {
  unsigned int count{ 0 };
};
using status = std::variant<count_down, enter_mash, finish>;

struct updater
{
  input_manager& im;

  status operator()(count_down& v) {
    if (std::chrono::steady_clock::now() < v.time_limit)
      return count_down{ v };
    using namespace std::literals::chrono_literals;
    return enter_mash{ std::chrono::steady_clock::now() + 5s };
  }

  status operator()(enter_mash& v) {
    if (v.time_limit <= std::chrono::steady_clock::now())
      return finish{ v.count };
    v.count += im.get_enter_count();
    return enter_mash{ v };
  }

  status operator()(finish& v) { return finish{ v }; }
};

struct render
{
  void operator()(count_down& v) {
    std::cout << std::chrono::ceil<std::chrono::milliseconds>(v.time_limit - std::chrono::steady_clock::now()).count() << std::flush;
  }

  void operator()(enter_mash& v) {
    std::cout << "!!!Smash Enter!!!\n" <<
                 "time_limit: " << std::chrono::ceil<std::chrono::milliseconds>(v.time_limit - std::chrono::steady_clock::now()).count() << " ms\n" << std::flush;
  }

  void operator()(finish&) { }
};

}
}

using namespace std::literals::chrono_literals;
struct state::gaming::impl
{
  game::status value{ game::count_down{std::chrono::steady_clock::now() + 3s} };
};

state::gaming::gaming(input_manager& im)
  : pimpl{ new impl{} }
{
  im.set_native(true);
}

status updater::operator()(state::gaming& v)
{
  if (std::holds_alternative<game::finish>(v.pimpl->value))
    return state::result{ im,  std::get<game::finish>(v.pimpl->value).count };
  v.pimpl->value = std::visit(game::updater{ im }, v.pimpl->value);
  return state::gaming{ v };
}

void render::operator()(state::gaming& v)
{
  std::visit(game::render{}, v.pimpl->value);
}

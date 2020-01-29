#include "updater.h"
#include "render.h"

#include <chrono>
#include <iostream>
#include <utility>
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

  status operator()(const count_down& v) {
    using namespace std::literals::chrono_literals;
    const auto now{ std::chrono::steady_clock::now() };
    if (v.time_limit <= now)
      return enter_mash{ now + 5s };
    return v;
  }

  status operator()(const enter_mash& v) {
    if (v.time_limit <= std::chrono::steady_clock::now())
      return finish{ v.count };
    auto res{ v };
    res.count += im.get_enter_count();
    return res;
  }

  status operator()(finish& v) { return v; }
};

struct render
{
  void operator()(const count_down& v) {
    std::cout << std::chrono::ceil<std::chrono::seconds>(v.time_limit - std::chrono::steady_clock::now()).count() << std::flush;
  }

  void operator()(const enter_mash& v) {
    std::cout << "!!!Smash Enter!!!\n" <<
                 "time_limit: " << std::chrono::ceil<std::chrono::seconds>(v.time_limit - std::chrono::steady_clock::now()).count() << " s\n" << std::flush;
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
{ im.set_native(true); }

state::gaming::~gaming() { delete pimpl; }

state::gaming::gaming(const gaming& g)
  : pimpl{new impl{*g.pimpl}}
{ }

state::gaming& state::gaming::operator=(const gaming& g)
{
  auto tmp{ pimpl };
  pimpl = new impl{ *g.pimpl };
  delete tmp;
  return *this;
}

state::gaming::gaming(gaming&& g) noexcept
  : pimpl{ g.pimpl }
{
  g.pimpl = nullptr;
}

state::gaming& state::gaming::operator=(gaming&& g) noexcept
{
  delete pimpl;
  pimpl = g.pimpl;
  g.pimpl = nullptr;
  return *this;
}

status updater::operator()(const state::gaming& v)
{
  if (std::holds_alternative<game::finish>(v.pimpl->value))
    return state::result{ im,  std::get<game::finish>(v.pimpl->value).count };
  auto res{ v };
  res.pimpl->value = std::visit(game::updater{ im }, res.pimpl->value);
  return res;
}

void render::operator()(const state::gaming & v)
{
  std::visit(game::render{}, v.pimpl->value);
}

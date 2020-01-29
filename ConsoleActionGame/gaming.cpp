#include "updater.h"
#include "render.h"

#include <chrono>
#include <iostream>
#include <random>
#include <utility>
#include <variant>

namespace
{
namespace game
{

struct count_down
{
  std::chrono::steady_clock::time_point time_limit;

  bool operator==(const count_down& cd) const noexcept { return false; }
};
struct enter_mash
{
  std::chrono::steady_clock::time_point time_limit;
  std::int_fast32_t count{ 0 };

  bool operator==(const enter_mash& em) const noexcept { return false; }
};
struct enter_just
{
  std::chrono::steady_clock::time_point time_limit;

  bool operator==(const enter_just& rhs) const noexcept { return false; }
};
struct grow_char
{
  std::uint_fast32_t hp;
  std::uint_fast32_t atack;

  bool operator==(const grow_char& rhs) const noexcept { return hp == rhs.hp && atack == rhs.atack; }
};
struct finish {
  std::int_fast32_t count{};

  bool operator==(const finish& f) const noexcept { return count == f.count; }
};

using status = std::variant<count_down, enter_mash, enter_just, grow_char, finish>;

struct updater
{
  input_manager& im;

  status operator()(const count_down& v) {
    using namespace std::literals::chrono_literals;
    const auto now{ std::chrono::steady_clock::now() };
    if (v.time_limit <= now)
      return grow_char{ 50, 1 };
    return v;
  }

  status operator()(const enter_mash& v) {
    if (v.time_limit <= std::chrono::steady_clock::now())
      return finish{ v.count };
    auto res{ v };
    res.count += im.get_enter_count();
    return res;
  }

  status operator()(const enter_just& v) {
    if (im.get_enter_count())
      return finish{ static_cast<std::int_fast32_t>(std::chrono::ceil<std::chrono::milliseconds>(v.time_limit - std::chrono::steady_clock::now()).count()) };
    return v;
  }

  status operator()(const grow_char& v) {
    if (!im.get_enter_count())
      return v;
    std::uniform_int_distribution<std::uint_fast32_t> ud{ 0, 10 };
    std::default_random_engine re{ std::random_device{}() };
    const auto damage{ ud(re) };
    if (v.hp <= damage)
      return finish{ static_cast<std::int_fast32_t>(v.atack) };
    auto res{ v };
    res.hp -= damage;
    res.atack += ud(re);
    return res;
  }

  status operator()(finish& v) { return v; }
};

struct render
{
  void operator()(const count_down& v) {
    std::cout << "Are you ready to smash enter key?\n" <<
                 "Count down: " << std::chrono::ceil<std::chrono::seconds>(v.time_limit - std::chrono::steady_clock::now()).count() << std::flush;
  }

  void operator()(const enter_mash& v) {
    std::cout << "!!!Smash Enter!!!\n" <<
                 "time_limit: " << std::chrono::ceil<std::chrono::seconds>(v.time_limit - std::chrono::steady_clock::now()).count() << " s\n" << std::flush;
  }

  void operator()(const enter_just& v) {
    std::cout << "!!!Just Push Enter!!!\n" <<
                 "Count down: " << std::chrono::ceil<std::chrono::milliseconds>(v.time_limit - std::chrono::steady_clock::now()).count() << " ms\n" << std::flush;
  }

  void operator()(const grow_char& v) {
    std::cout << "atack: " << v.atack << " hp: " << v.hp << "\n\n";
    if (v.atack < 15) {
      std::cout << R"(
　　　　（Ｕ＾ω＾）　わんわんお！
　　Ｃ／　　　　ｌ
　　　し－し－Ｊ)";
    } else if (v.atack < 30) {
      std::cout << R"(
　　　　　　　/ヽ　　　　/ヽ
　　　　　　 　/∧ヽ　＿/∧ヽ
　　　　　 　 /　　　　　　　　 ヽ
　　　　 　／　　　　　　　　　　|　　ｸｳｰﾝ・・・
　　　　　 ●　　　●　　　　　　|
　　　 ／　　　　　 　　　　　　ゝゝ
　　 |▼　　　　　　　　　 ／　ヽ
　　　ヽ人＿＿　　　　　 ／　／　ヽ　　
　　　　　ヽ＿＿＿　　／　／　　　　ヽ
　　　　　　　　 |＿正夫／　　　　　　　ヽ
　　　　　　　　/　　　　　　　　　　　　　　＼
　　　　　　　　|"　　　　””　　　　　　　　　　＼
　　　　　　　　|　　　　　　　　　　　　　　　　　　＼
　　　　　　　　|　　　　　　　　　　　　　　　　 　　　｜
　　　　　　　　＼　　|　　　　　　　　　　　　　　 　　｜
　　　　　　　　　｜＼　|　　　　　/　　　　　　　　　 ｜
　　　　　　　　　｜　＼|　　　　/　/　　　　　　　　 　|
　　　　　　　　　｜　｜/|　　　|　 |　　　　　　　　　　|　　　
　　　　　　　　　｜　｜|｜　　|　　|　　　　　　　　　　|＿／￣￣/ 　　　
　　　　　　　　／　　 | ＼|　　|　　 ＼　　　　　　　　|＼＿＿_／ 　　　　
　　　　　　（（（＿_|　 （（（___　|-　（（________________／
)";
    } else {
      std::cout << R"(
　　　Ａ__Ａ  
 　　(o・ω・)  
/ヽヘし 　　 !   
＼/ヽｕ─ｕ'
)";
    }
    std::cout << std::endl;
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
  if (this == &g)
    return *this;
  delete pimpl;
  pimpl = g.pimpl;
  g.pimpl = nullptr;
  return *this;
}

bool state::gaming::operator==(const gaming& g) const noexcept
{
  return pimpl->value == g.pimpl->value;
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

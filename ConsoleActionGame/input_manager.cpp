#include "input_manager.h"

#include <array>
#include <iostream>
#include <mutex>
#include <optional>
#include <stdexcept>
#include <string>
#include <variant>

#include <Windows.h>

namespace
{

class console_mode_guard
{
public:
  console_mode_guard(HANDLE& input_handle, DWORD new_flags)
    : handle{ input_handle },
    console_mode{ new_flags },
    console_old_mode{}
  {
    if (!GetConsoleMode(handle, &console_old_mode))
      throw std::runtime_error{ "console_initalizaiton: Fail GetConsoleMode" };
    if (!SetConsoleMode(handle, console_mode))
      throw std::runtime_error{ "console_initalizaiton: Fail SetConsoleMode" };
  }

  ~console_mode_guard() noexcept
  {
    SetConsoleMode(handle, console_old_mode);
  }

  console_mode_guard(console_mode_guard&& cmg) noexcept
    : handle{ cmg.handle },
    console_mode{ cmg.console_mode },
    console_old_mode{ cmg.console_old_mode }
  {
    cmg.console_old_mode = cmg.console_mode;
  }

  console_mode_guard& operator=(console_mode_guard&& cmg) noexcept
  {
    if (this == &cmg)
      return *this;
    SetConsoleMode(handle, console_old_mode);
    handle = cmg.handle;
    console_mode = cmg.console_mode;
    console_old_mode = cmg.console_old_mode;
    cmg.console_old_mode = cmg.console_mode;
    return *this;
  }

private:
  HANDLE& handle;
  DWORD console_mode;
  DWORD console_old_mode;
};

struct NormalInput
{
  std::string row_input{};
};
struct NativeInput
{
  console_mode_guard cmg;
  unsigned int enter_count{};
  std::string row_input{};
};

struct input_updater
{
  HANDLE& input_handle;

  void operator()(NormalInput& v)
  {
    DWORD read_count;
    if (!GetNumberOfConsoleInputEvents(input_handle, &read_count) || !read_count)
      return;
    std::array<char, 1 << 8> inputs;
    if (!ReadConsole(input_handle, inputs.data(), static_cast<DWORD>(inputs.size()), &read_count, nullptr))
      throw std::runtime_error{ "input_manager: fail ReadConsoleInput" };
    v.row_input.append(inputs.cbegin(), std::next(inputs.cbegin(), read_count));
  }

  void operator()(NativeInput& v)
  {
    DWORD read_count;
    if (!GetNumberOfConsoleInputEvents(input_handle, &read_count) || !read_count)
      return;
    std::array<INPUT_RECORD, 1 << 8> inputs;
    if (!ReadConsoleInput(input_handle, inputs.data(), static_cast<DWORD>(inputs.size()), &read_count))
      throw std::runtime_error{ "input_manager: fail ReadConsoleInput" };

    auto& row_input{ v.row_input };
    for_each(inputs.cbegin(), next(inputs.cbegin(), read_count), [&](const INPUT_RECORD& e) {
      switch (e.EventType) {
      case KEY_EVENT: // keyboard input
      {
        const auto& key{ e.Event.KeyEvent };
        row_input.push_back(key.uChar.AsciiChar);
        if (!key.bKeyDown && key.wVirtualKeyCode == VK_RETURN) // this needed for detect key up
          ++v.enter_count;
      }
      break;

      case MOUSE_EVENT: [[fallthrough]];
      case WINDOW_BUFFER_SIZE_EVENT: [[fallthrough]];
      case FOCUS_EVENT: [[fallthrough]];
      case MENU_EVENT:
        break;

      default:
        throw std::runtime_error{ "input_manager: Unexpected EventType input" };
      }
      });
  }
};

struct input_reseter
{
  void operator()(NormalInput& v) noexcept
  {
    v = {};
  }

  void operator()(NativeInput& v) noexcept
  {
    v = { std::move(v.cmg) };
  }
};

template<typename F, typename T>
struct visit_guard
{
  visit_guard(F f_, T& v_)
    : f{ f_ },
      v{ v_ }
  {}

  ~visit_guard() noexcept
  {
    std::visit(f, v);
  }

private:
  F f;
  T& v;
};

}

struct input_manager::impl
{
  HANDLE input_handle;
  std::variant<NormalInput, NativeInput> value;
  std::mutex m;

  impl(HANDLE& ih) noexcept
    : input_handle{ ih },
      value{ NormalInput{} },
      m {}
  { }
};

input_manager::input_manager(HANDLE& input_handle)
  : pimpl{ new impl{ input_handle } }
{}

input_manager::~input_manager() noexcept { delete pimpl; }

void input_manager::update()
{
  std::lock_guard lg{ pimpl->m };
  std::visit(input_updater{ pimpl->input_handle }, pimpl->value);
}

void input_manager::set_native(bool enable)
{
  std::lock_guard lg{ pimpl->m };
  if (enable) {
    pimpl->value = NativeInput{ { pimpl->input_handle, ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS }, 0 };
  } else {
    pimpl->value = NormalInput{ };
  }
}

unsigned int input_manager::get_enter_count() noexcept
{
  std::lock_guard lg{ pimpl->m };
  {
    visit_guard vg{ input_reseter{}, pimpl->value };
    if (std::holds_alternative<NativeInput>(pimpl->value))
      return std::get<NativeInput>(pimpl->value).enter_count;
    return 1;
  }
}

std::string input_manager::getline()
{
  std::lock_guard lg{ pimpl->m };
  {
    visit_guard vg{ input_reseter{}, pimpl->value };
    return std::visit([](const auto& v) { return v.row_input; }, pimpl->value);
  }
}
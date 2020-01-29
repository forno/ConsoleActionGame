#include "input_manager.h"

#include <array>
#include <iostream>
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
    std::string row_input;
  };
  struct NativeInput
  {
    console_mode_guard cmg;
    unsigned int enter_count;
    std::string row_input;
  };

  struct input_updater
  {
    HANDLE& input_handle;

    void operator()(NormalInput& v)
    {
      std::array<char, 1 << 8> inputs;
      DWORD read_count;
      if (!ReadConsole(input_handle, inputs.data(), static_cast<DWORD>(inputs.size()), &read_count, nullptr))
        throw std::runtime_error{ "input_manager: fail ReadConsoleInput" };
      v.row_input.assign(inputs.cbegin(), std::next(inputs.cbegin(), read_count));
    }

    void operator()(NativeInput& v)
    {
      std::array<INPUT_RECORD, 1 << 8> inputs;
      DWORD read_count;
      if (!ReadConsoleInput(input_handle, inputs.data(), static_cast<DWORD>(inputs.size()), &read_count))
        throw std::runtime_error{ "input_manager: fail ReadConsoleInput" };

      auto& row_input{ v.row_input };
      for_each(inputs.cbegin(), next(inputs.cbegin(), read_count), [&](const INPUT_RECORD& e) {
        switch (e.EventType) {
        case KEY_EVENT: // keyboard input
        {
          const auto& key{ e.Event.KeyEvent };
          row_input.push_back(key.uChar.AsciiChar);
          if (!key.bKeyDown && key.wVirtualKeyCode == VK_RETURN)
            ++v.enter_count;
        }
        break;

        case MOUSE_EVENT:
          [[fallthrough]];
        case WINDOW_BUFFER_SIZE_EVENT:
          [[fallthrough]];
        case FOCUS_EVENT:
          [[fallthrough]];
        case MENU_EVENT:
          break;

        default:
          throw std::runtime_error{ "input_manager: Unexpected EventType input" };
        }
      });
    }
  };
}

class input_manager::impl
{
public:
  impl(HANDLE& ih) noexcept
    : input_handle{ ih },
      value{ NormalInput{} }
  { }

  void update()
  {
    std::visit(input_updater{ input_handle }, value);
  }

  void set_native(bool enable)
  {
    if (enable) {
      value = NativeInput{ { input_handle, ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS }, 0 };
    } else {
      value = NormalInput{ };
    }
  }

  unsigned int get_enter_count() noexcept
  {
    if (std::holds_alternative<NativeInput>(value)) {
      return std::get<NativeInput>(value).enter_count;
    }
    return 1;
  }

  std::string getline()
  {
    return std::visit([](const auto& v) { return v.row_input; }, value);
  }

private:
  HANDLE input_handle;
  std::variant<NormalInput, NativeInput> value;
};

input_manager::input_manager(HANDLE& input_handle)
  : pimpl{ new impl{ input_handle } }
{}

input_manager::~input_manager() noexcept { delete pimpl; }

void input_manager::update()
{
  pimpl->update();
}

void input_manager::set_native(bool enable)
{
  pimpl->set_native(enable);
}

unsigned int input_manager::get_enter_count() noexcept
{
  return pimpl->get_enter_count();
}

std::string input_manager::getline()
{
  return pimpl->getline();
}
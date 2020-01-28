#include "input_manager.h"

#include <array>
#include <stdexcept>
#include <optional>
#include <variant>

#include <Windows.h>

namespace {

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
  };
}

class input_manager::impl
{
  static constexpr auto input_buf_size{ 1 << 8 };
public:
  impl(HANDLE& ih) noexcept
    : input_handle{ ih },
      value{ NormalInput{} }
  { }

  void update()
  {
    if (std::holds_alternative<NativeInput>(value)) {
      auto& v{ std::get<NativeInput>(value) };
      std::array<INPUT_RECORD, input_buf_size> inputs;
      DWORD read_count;
      if (!ReadConsoleInput(input_handle, inputs.data(), static_cast<DWORD>(inputs.size()), &read_count))
        throw std::runtime_error{ "input_manager: fail ReadConsoleInput" };

      for_each(inputs.cbegin(), next(inputs.cbegin(), read_count), [&](const INPUT_RECORD& e) {
        switch (e.EventType)
        {
        case KEY_EVENT: // keyboard input
        {
          const auto& key{ e.Event.KeyEvent };
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

private:
  HANDLE input_handle;
  std::variant<NormalInput, NativeInput> value;
};

input_manager::input_manager(HANDLE& input_handle)
  : pimpl{ new impl{ input_handle } }
{}

input_manager::~input_manager() noexcept { delete pimpl; }

void input_manager::update() {
  pimpl->update();
}

unsigned int input_manager::get_enter_count() noexcept {
  return pimpl->get_enter_count();
}
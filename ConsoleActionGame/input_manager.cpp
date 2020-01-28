#include "input_manager.h"

#include <array>
#include <stdexcept>
#include <Windows.h>

class input_manager::impl
{
  static constexpr auto input_buf_size{ 1 << 8 };
public:
  impl(HANDLE& ih) noexcept
    : input_handle{ ih },
      enter_count{ 0 }
  { }

  void update()
  {
    std::array<INPUT_RECORD, input_buf_size> inputs;
    DWORD read_count;
    if (!ReadConsoleInput(
      input_handle,      // input buffer handle
      inputs.data(),     // buffer to read into
      inputs.size(), // size of read buffer
      &read_count)) // number of records read
      throw std::runtime_error{ "input_manager: fail ReadConsoleInput" };

    for_each(inputs.cbegin(), next(inputs.cbegin(), read_count), [&](const INPUT_RECORD& e) {
      switch (e.EventType)
      {
      case KEY_EVENT: // keyboard input
      {
        const auto& key{ e.Event.KeyEvent };
        if (!key.bKeyDown && key.wVirtualKeyCode == VK_RETURN)
          ++enter_count;
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

  unsigned int get_enter_count() noexcept
  {
    return enter_count;
  }

private:
  HANDLE input_handle;
  unsigned int enter_count;
};

input_manager::input_manager(HANDLE& input_handle)
  : pimpl{ new impl{ input_handle } } {}

input_manager::~input_manager() noexcept { delete pimpl; }

void input_manager::update()
{
  pimpl->update();
}

unsigned int input_manager::get_enter_count() noexcept
{
  return pimpl->get_enter_count();
}
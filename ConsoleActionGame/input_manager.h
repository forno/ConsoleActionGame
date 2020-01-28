#pragma once

#include <Windows.h>

class input_manager
{
  input_manager(HANDLE& input_handle);
  ~input_manager() noexcept;
  void update();
  unsigned int get_enter_count() noexcept;

private:
  class impl;
  impl* pimpl;
};

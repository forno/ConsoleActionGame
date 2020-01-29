#pragma once

#include <string>
#include <Windows.h>

class input_manager
{
public:
  input_manager(HANDLE& input_handle);
  ~input_manager() noexcept;
  void update();
  unsigned int get_enter_count() noexcept;
  std::string getline();

private:
  class impl;
  impl* pimpl;
};

#pragma once

#include <string>
#define NOMINMAX
#include <Windows.h>
#undef NOMINMAX

class input_manager
{
public:
  input_manager(HANDLE& input_handle);
  ~input_manager() noexcept;
  void set_native(bool enable);
  void update();
  unsigned int get_enter_count() noexcept;
  std::string getline();

private:
  struct impl;
  impl* pimpl;
};

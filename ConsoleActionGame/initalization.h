#pragma once

#include <windows.h>

#ifdef __cplusplus

class console_initalizaiton
{
public:
	console_initalizaiton(HANDLE& input_handle);
	~console_initalizaiton();
private:
	class impl;
	impl* pimpl;
};

#else

extern HANDLE input_handle;

[[noreturn]]
void myexit();

bool initalize();

#endif

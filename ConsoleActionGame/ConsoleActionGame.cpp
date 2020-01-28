#include <chrono>
#include <iostream>
#include <windows.h>

#include "initalization.h"
#include "gaming.h"
#include "status.h"
#include "title.h"
#include "result.h"

int main(void)
{
    HANDLE input_handle;
    if ((input_handle = GetStdHandle(STD_INPUT_HANDLE)) == INVALID_HANDLE_VALUE)
    	return;
    console_initalizaiton ci{ input_handle };

    Status s{ Title };
    Data d;

    while (true) {
        switch (s) {
            case Title:
                s = title_func(&d);
                break;
		    case Gaming:
                s = gaming_func(&d);
                break;
            case Result:
                s = result_func(&d);
                break;
            case Finish:
                return 0;
        }
    }
    return 0;
}

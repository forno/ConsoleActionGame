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
    if (initalize())
        return 0;

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
                myexit();
                return 0;
        }
    }
    myexit();
    return 0;
}

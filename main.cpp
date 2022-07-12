#include <iostream>
#include "kw.h"
#include "unicode.hpp"
using namespace std;

int main()
{

    kw::init("words.txt");
    {
        char str[1024] = u8"my shiter go";
        printf("===>:%s\r\n", str);
        if (kw::hexie(str) == 0)
        {
            printf("===>:ok\r\n");
        }
        else
        {
            printf("===>:err %s\r\n", str);
        }
    }

    // {
    //     char str[1024] = u8"123𪜀𪜀爱𪜀𪜀321";
    //     printf("===>:%s\r\n", str);
    //     if (kw::hexie(str) == 0)
    //     {
    //         printf("===>:ok\r\n");
    //     }
    //     else
    //     {
    //         printf("===>:err %s\r\n", str);
    //     }
    // }

    // {
    //     char str[1024] = u8"123愛る𪜀321";
    //     printf("===>:%s\r\n", str);
    //     char hint[1024] = "";
    //     if (kw::check(str, hint) == 0)
    //     {
    //         printf("===>:ok\r\n");
    //     }
    //     else
    //     {
    //         printf("===>:err hint:%s\r\n", hint);
    //     }
    // }
}

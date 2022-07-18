#include <iostream>
#include "kw.h"
#include "unicode.hpp"
using namespace std;

int main()
{

    kw::init("words.txt");
    {
        char str[1024] = u8"Shif 22////tfuckt";
        printf("===>:%s\r\n", str);
        if (kw::hexie(str) == 0)
        {
            printf("===>:ok\r\n");
        }
        else
        {
            printf("===>:%s\r\n", str);
        }
    }

    {
        char str[1024] = u8"falundafa  flg   chinaliberala123sh itw321";
        printf("===>:%s\r\n", str);
        char hint[1024] = "";
        if (kw::check(str, hint) == 0)
        {
            printf("===>:ok\r\n");
        }
        else
        {
            printf("===>:err hint:%s\r\n", hint);
        }
    }

}

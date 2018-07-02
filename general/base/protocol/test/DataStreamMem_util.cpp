#include <iostream>

#include "../util/DataStreamSerializer.h"

int main()
{
    int8_t x = 12;
    int32_t y = 51;
    std::string word = "this is my mesage for all the players...";
    DataStreamSerializer s_be(static_cast<const unsigned int>(sizeof(x) + sizeof(y) + word.length() + 4));
    s_be << x;
    s_be << y;
    s_be << word;

    s_be.setOpen();

    int8_t v = 0;
    int32_t w = 0;
    std::string caught;

    s_be >> v;
    s_be >> w;
    s_be >> caught;


    return 0;
}

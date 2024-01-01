#ifndef RESULTDATA_H
#define RESULTDATA_H

#include <cstdlib>

struct ResultData {
    char s[65535];
    std::size_t len = 0;
};

#endif
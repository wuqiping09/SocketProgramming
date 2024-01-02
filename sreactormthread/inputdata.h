#ifndef INPUTDATA_H
#define INPUTDATA_H

#include <cstdlib>

struct InputData {
    char s[65535];
    std::size_t len = 0;
};

#endif
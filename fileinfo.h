#ifndef FILEINFO_H
#define FILEINFO_H

#include <string>

struct FileInfo {
    char filename[100];
    std::size_t filesize;
};

#endif
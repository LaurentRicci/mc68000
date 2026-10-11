#pragma once
#include <string>
#include <filesystem>

struct FileCleanup
{
    std::string path;
    FileCleanup(const char* p) : path(p)
    {
    }
    ~FileCleanup()
    {
        std::filesystem::remove(path);
    }
};

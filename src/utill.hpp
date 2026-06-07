#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <assert.h>

namespace utill{
    std::string read_file(const std::string& path);
    void write_file(const std::string& path, const std::string& data);
    bool file_exist(const std::string& path);
}
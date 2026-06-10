#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <filesystem>
#include <assert.h>
#include <cstdint>

namespace utill{
    std::string read_file(const std::string& path);
    uint32_t read_file_binary(const std::string& path);
    std::vector<uint8_t> read_file_binary_vector(const std::string& path);
    void write_file_binary(const std::string& path, const uint8_t data[], size_t size_of_data);
    void write_file_binary(const std::string& path, const uint32_t data);
    bool file_exist(const std::string& path);
}
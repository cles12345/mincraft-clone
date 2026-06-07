#include "utill.hpp"

namespace utill{
    std::string read_file(const std::string& path)
    {
        std::ifstream file(path);
        if (file.fail())
        {
            std::cout << "Failed to read file " << path  << "\n";
            assert(false);
        }
        std::stringstream buffer;

        buffer << file.rdbuf();

        file.close();
        
        return buffer.str();
    }

    void write_file(const std::string& path, const std::string& data)
    {
        std::ofstream file(path);
        if (file.fail())
        {
            std::cout << "Failed to write file " << path  << "\n";
            assert(false);
        }

        file << data;

        file.close();
    }

    bool file_exist(const std::string& path)
    {
        std::ifstream file(path);
        return file.is_open();
    }
}
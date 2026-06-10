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

    uint32_t read_file_binary(const std::string& path)
    {
        std::ifstream file(path, std::ios::binary);
        if (file.fail())
        {
            std::cout << "Failed to read file " << path  << "\n";
            assert(false);
        }

        uint32_t data = 0;
        
        file.read(reinterpret_cast<char*>(&data), sizeof(uint32_t));

        file.close();
        
        return data;
    }

    std::vector<uint8_t> read_file_binary_vector(const std::string& path)
    {
        std::ifstream file(path, std::ios::binary);
        if (file.fail())
        {
            std::cout << "Failed to read file " << path  << "\n";
            assert(false);
        }

        size_t size = std::filesystem::file_size(path);
        std::vector<uint8_t> data(size);

        file.read(reinterpret_cast<char*>(data.data()), size);
        
        file.close();
        
        return data;
    }

    void write_file_binary(const std::string& path, const uint8_t data[], size_t size_of_data)
    {
        std::ofstream file(path, std::ios::binary);
        if (file.fail())
        {
            std::cout << "Failed to write file " << path  << "\n";
            assert(false);
        }

        file.write(reinterpret_cast<const char*>(data), size_of_data);

        file.close();
    }

    void write_file_binary(const std::string& path, const uint32_t data)
    {
        std::ofstream file(path, std::ios::binary);
        if (file.fail())
        {
            std::cout << "Failed to write file " << path  << "\n";
            assert(false);
        }

        file.write(reinterpret_cast<const char*>(&data), sizeof(uint32_t));

        file.close();
    }

    bool file_exist(const std::string& path)
    {
        std::ifstream file(path);
        return file.is_open();
    }
}
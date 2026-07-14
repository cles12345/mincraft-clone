#pragma once
#include <chrono>
#include <cassert>
#include <cstdlib> 
#include <new>

enum class time_unit 
{
    MS,
    S,
};

class Profiler
{
    public:
        void begin();
        void stop();
        double time_taken(time_unit unit = time_unit::MS);
        void reset();
        static size_t memory_allocated;
        static size_t peak_memory_allocated;

    private:
        std::chrono::high_resolution_clock::time_point start, end;
        std::chrono::duration<double> duration;
        bool is_running = false;
};

void* operator new(std::size_t size);
void* operator new[](std::size_t size);
void operator delete(void* ptr) noexcept;
void operator delete[](void* ptr) noexcept;
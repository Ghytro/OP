//код для тестирования функций

#include <iostream>
#include <chrono>
#include "pvector.h"

void test_accumulate()
{
    constexpr int v_size = 1e9;
    PVector<int> v(v_size, 1);
    auto start = std::chrono::high_resolution_clock::now();
    int answer1 = v.accumulate();
    double ms1 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count() / 1000.0;
    start = std::chrono::high_resolution_clock::now();
    int answer2 = v.accumulate_singlethread();
    double ms2 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count() / 1000.0;
    std::cout << "answer1: " << answer1 << std::endl;
    std::cout << "answer2: " << answer2 << std::endl;
    std::cout << "time parallel: " << ms1 << " seconds" << std::endl;
    std::cout << "time singlethread: " << ms2 << " seconds" << std::endl;
}

void test_min()
{
    constexpr int v_size = 1e9;
    PVector<int> v(v_size, 1);
    v[1000] = 0;
    auto start = std::chrono::high_resolution_clock::now();
    int answer1 = v.min();
    double ms1 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count() / 1000.0;
    start = std::chrono::high_resolution_clock::now();
    int answer2 = v.min_singlethread();
    double ms2 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count() / 1000.0;
    std::cout << "answer1: " << answer1 << std::endl;
    std::cout << "answer2: " << answer2 << std::endl;
    std::cout << "time parallel: " << ms1 << " seconds" << std::endl;
    std::cout << "time singlethread: " << ms2 << " seconds" << std::endl;
}

void test_max()
{
    constexpr int v_size = 1e9;
    PVector<int> v(v_size, 1);
    v[1000] = 2;
    auto start = std::chrono::high_resolution_clock::now();
    int answer1 = v.max();
    double ms1 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count() / 1000.0;
    start = std::chrono::high_resolution_clock::now();
    int answer2 = v.max_singlethread();
    double ms2 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count() / 1000.0;
    std::cout << "answer1: " << answer1 << std::endl;
    std::cout << "answer2: " << answer2 << std::endl;
    std::cout << "time parallel: " << ms1 << " seconds" << std::endl;
    std::cout << "time singlethread: " << ms2 << " seconds" << std::endl;
}

int main()
{
    test_max();
}

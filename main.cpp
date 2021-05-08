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

void test_sort()
{
    constexpr int v_size = 1e9;
    std::srand(std::time(0));
    PVector<char> v(v_size);
    for (int i = 0; i < v.size(); ++i)
         v[i] = std::rand() % 100;
    auto start = std::chrono::high_resolution_clock::now();
    v.sort();
    double ms_par = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count() / 1000.0;
    if (!std::is_sorted(v.begin(), v.end()))
        std::cout << "Something wrong with sorting algorithm" << std::endl;
    std::cout << "Time multithread: " << ms_par << std::endl;
    std::srand(std::time(0));
    for (int i = 0; i < v.size(); ++i)
        v[i] = std::rand() % 100;
    start = std::chrono::high_resolution_clock::now();
    std::sort(v.begin(), v.end());
    double ms_single = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count() / 1000.0;
    if (!std::is_sorted(v.begin(), v.end()))
        std::cout << "Something wrong with std::sort" << std::endl;
    std::cout << "Time singlethread: " << ms_single << std::endl;
}

int main()
{
    test_sort();
}

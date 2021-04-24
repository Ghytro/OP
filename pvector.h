#ifndef PVECTOR_H
#define PVECTOR_H

#include <cstdint>
#include <cstdlib>
#include <thread>
#include <algorithm>
#include <numeric>

template <class T>
class PVector
{
private:
    T *__elements;
    uint64_t __size = 0;
    uint64_t __capacity = 0;
    const unsigned __hardware_concurrency = std::thread::hardware_concurrency();

public:
    static constexpr uint64_t npos = -1;

    explicit PVector(const uint64_t &init_size = 0, const T &fill = T()):
    __size(init_size),
    __capacity(init_size) //initialisation before counting real value
    {
        if ((__capacity & (__capacity - 1)) != 0 && __capacity != 0) //300 iq capacity computation
        {
            __capacity |= __capacity >> 1;
            __capacity |= __capacity >> 2;
            __capacity |= __capacity >> 4;
            __capacity |= __capacity >> 8;
            __capacity |= __capacity >> 16;
            __capacity |= __capacity >> 32;
            ++__capacity;
        }
        this->__elements = new T[__capacity];

        for (T *it = __elements; it != __elements + init_size; ++it)
            *it = fill;
        for (T *it = __elements + init_size; it != __elements + __capacity; ++it)
            *it = T();
    }

    PVector(const PVector &other)
    {
        if (this->__size != other.__size)
        {
            this->__size = other.__size;
            if (this->__capacity != other.__capacity)
            {
                this->__capacity = other.__capacity;
                this->__elements = (T*)std::realloc(this->__elements, this->__capacity * sizeof(T));
            }
        }
        for (uint64_t i = 0; i < this->__size; ++i)
            this->__elements[i] = other.__elements[i];
    }

    virtual ~PVector()
    {
        delete[] this->__elements;
    }

    uint64_t size() const noexcept {return this->__size;}
    bool empty() const noexcept {return this->__size != 0;}
    uint64_t capacity() const noexcept {return this->__capacity;}

    T front() const {return this->__elements[0];}
    T& front() {return this->__elements[0];}

    T back() const {return this->__elements[this->__size - 1];}
    T& back() {return this->__elements[this->__size - 1];}

    T operator[](const uint64_t &index) const {return this->__elements[index];}
    T& operator[](const uint64_t &index) {return this->__elements[index];}

    void push_back(const T &value)
    {
        if (this->__size == this->__capacity)
        {
            this->__elements = (T*)std::realloc(this->__elements, (this->__capacity <<= 1) * sizeof(T));
            if (this->__elements == NULL)
                throw std::bad_alloc();
        }
        this->__elements[this->__size++] = value;
    }

    void pop_back()
    {
        if (this->__size-- == (this->__capacity >> 1) + 1)
            this->__elements = (T*)std::realloc(this->__elements, (this->__capacity >>= 1) * sizeof(T));
    }

    void pop_back(T &ref)
    {
        ref = this->__elements[--this->__size];
        if (this->__size == (this->__capacity >> 1) + 1)
            this->__elements = (T*)std::realloc(this->__elements, (this->__capacity >>= 1) * sizeof(T));
    }

    uint64_t find(const T &value)
    {
        auto search_multithread = [](PVector *vec, const T &value, const uint64_t &start, const uint64_t &step, uint64_t &index)
        {
            for (uint64_t i = start; i < vec->size(); i += step)
            {
                if (index != PVector::npos)
                    return;
                if ((*vec)[i] == value)
                {
                    index = i;
                    return;
                }
            }
        };
        const unsigned max_workers = std::thread::hardware_concurrency();
        std::thread workers[max_workers];
        uint64_t index = PVector::npos;
        for (unsigned i = 0; i < max_workers; ++i)
            workers[i] = std::thread(search_multithread, this, std::cref(value), std::cref(i), std::cref(max_workers), std::ref(index));
        for (unsigned i = 0; i < max_workers; ++i)
            workers[i].join();
        return index;
    }

    bool contains(const T &value) {return this->find(value) != PVector::npos;}

    //only for benchmarks, delete in final version
    uint64_t find_singlethread(const T &value)
    {
        T *it = std::find(this->__elements, this->__elements + this->__size, value);
        if (it == this->__elements + this->__size)
            return PVector::npos;
        return it - this->__elements;
    }

    T accumulate(const T &init_value = T())
    {
        auto accumulate_interval = [&](uint64_t start, T &ans, const T &init_value) {
            T result = init_value;
            for (uint64_t i = start; i < this->__size; i += this->__hardware_concurrency)
                result  = result + this->__elements[i];
            ans = result;
        };

        T *temp_answers = new T[this->__hardware_concurrency];
        std::thread *workers = new std::thread[this->__hardware_concurrency];

        for (unsigned i = 0; i < this->__hardware_concurrency; ++i)
            workers[i] = std::thread(accumulate_interval, i, std::ref(temp_answers[i]), std::cref(init_value));
        for (unsigned i = 0; i < this->__hardware_concurrency; ++i)
            workers[i].join();

        T result = init_value;
        for (unsigned i = 0; i < this->__hardware_concurrency; ++i)
            result = result + temp_answers[i];

        delete[] temp_answers;
        delete[] workers;
        return result;
    }

    template <class Operation>
    T accumulate(Operation op, const T &init_value = T())
    {
        auto accumulate_interval = [&](uint64_t start, T &ans, const T &init_value) {
            T result = init_value;
            for (uint64_t i = start; i < this->__size; i += this->__hardware_concurrency)
                result  = op(result, this->__elements[i]);
            ans = result;
        };

        T *temp_answers = new T[this->__hardware_concurrency];
        std::thread *workers = new std::thread[this->__hardware_concurrency];

        for (unsigned i = 0; i < this->__hardware_concurrency; ++i)
            workers[i] = std::thread(accumulate_interval, i, std::ref(temp_answers[i]), std::cref(init_value));
        for (unsigned i = 0; i < this->__hardware_concurrency; ++i)
            workers[i].join();

        T result = init_value;
        for (unsigned i = 0; i < this->__hardware_concurrency; ++i)
            result = op(result, temp_answers[i]);

        delete[] temp_answers;
        delete[] workers;
        return result;
    }

    T accumulate_singlethread(const T &init_value = T())
    {
        return std::accumulate(this->__elements, this->__elements + this->__size, init_value);
    }

    T min()
    {
        auto find_min = [&](uint64_t start, T &ans) {
            T result = this->__elements[0];
            for (uint64_t i = start; i < this->__size; i += this->__hardware_concurrency)
                if (this->__elements[i] < result)
                    result = this->__elements[i];
            ans = result;
        };

        T *results = new T[this->__hardware_concurrency];
        std::thread *workers = new std::thread[this->__hardware_concurrency];

        for (unsigned i = 0; i < this->__hardware_concurrency; ++i)
            workers[i] = std::thread(find_min, i, std::ref(results[i]));
        for (unsigned i = 0; i < this->__hardware_concurrency; ++i)
            workers[i].join();

        T result = results[0];
        for (unsigned i = 1; i < this->__hardware_concurrency; ++i)
            if (results[i] < result)
                result = results[i];

        delete[] results;
        delete[] workers;
        return result;
    }

    template <class Operation>
    T min(Operation op)
    {
        auto find_min = [&](uint64_t start, T &ans) {
            T result = this->__elements[start];
            for (uint64_t i = start; i < this->__size; i += this->__hardware_concurrency)
                if (op(this->__elements[i], result))
                    result = this->__elements[i];
            ans = result;
        };

        T *results = new T[this->__hardware_concurrency];
        std::thread *workers = new std::thread[this->__hardware_concurrency];

        for (unsigned i = 0; i < this->__hardware_concurrency; ++i)
            workers[i] = std::thread(find_min, i, std::ref(results[i]));
        for (unsigned i = 0; i < this->__hardware_concurrency; ++i)
            workers[i].join();

        T result = results[0];
        for (unsigned i = 1; i < this->__hardware_concurrency; ++i)
            if (op(results[i], result))
                result = results[i];

        delete[] results;
        delete[] workers;
        return result;
    }

    T min_singlethread()
    {
        return (*std::min_element(this->__elements, this->__elements + this->__size));
    }

    T max()
    {
        auto find_max = [&](uint64_t start, T &ans) {
            T result = this->__elements[0];
            for (uint64_t i = start; i < this->__size; i += this->__hardware_concurrency)
                if (this->__elements[i] > result)
                    result = this->__elements[i];
            ans = result;
        };

        T *results = new T[this->__hardware_concurrency];
        std::thread *workers = new std::thread[this->__hardware_concurrency];

        for (unsigned i = 0; i < this->__hardware_concurrency; ++i)
            workers[i] = std::thread(find_max, i, std::ref(results[i]));
        for (unsigned i = 0; i < this->__hardware_concurrency; ++i)
            workers[i].join();

        T result = results[0];
        for (unsigned i = 1; i < this->__hardware_concurrency; ++i)
            if (results[i] > result)
                result = results[i];

        delete[] results;
        delete[] workers;
        return result;
    }

    template <class Operation>
    T max(Operation op)
    {
        auto find_max = [&](uint64_t start, T &ans) {
            T result = this->__elements[0];
            for (uint64_t i = start; i < this->__size; i += this->__hardware_concurrency)
                if (op(this->__elements[i], result))
                    result = this->__elements[i];
            ans = result;
        };

        T *results = new T[this->__hardware_concurrency];
        std::thread *workers = new std::thread[this->__hardware_concurrency];

        for (unsigned i = 0; i < this->__hardware_concurrency; ++i)
            workers[i] = std::thread(find_max, i, std::ref(results[i]));
        for (unsigned i = 0; i < this->__hardware_concurrency; ++i)
            workers[i].join();

        T result = results[0];
        for (unsigned i = 1; i < this->__hardware_concurrency; ++i)
            if (op(results[i], result))
                result = results[i];

        delete[] results;
        delete[] workers;
        return result;
    }

    T max_singlethread()
    {
        return (*std::max_element(this->__elements, this->__elements + this->__size));
    }
};

#endif // PVECTOR_H

#ifndef SIMPLE_ARRAY_H
#define SIMPLE_ARRAY_H
#include <stddef.h>

namespace utils{
template<typename T, size_t N>
struct SimpleArray
{
    T storage[N];

    // Default constructor
    constexpr SimpleArray() = default;

    // Fill constructor
    constexpr explicit SimpleArray(const T& value)
    {
        fill(value);
    }
	constexpr SimpleArray(const T (&input)[N])
    {
        for (size_t i = 0; i < N; ++i)
            storage[i] = input[i];
    }

    // Initializer-list constructor
   /* constexpr SimpleArray(std::initializer_list<T> list)
    {
        size_t i = 0;

        for (const T& value : list)
        {
            if (i >= N)
                break;

            storage[i++] = value;
        }

        // Default-initialize remaining elements
        for (; i < N; ++i)
            storage[i] = T{};
    }*/

    // Copy constructor
    constexpr SimpleArray(const SimpleArray&) = default;

    // Move constructor
    constexpr SimpleArray(SimpleArray&&) = default;

    // Copy assignment
    constexpr SimpleArray& operator=(const SimpleArray&) = default;

    // Move assignment
    constexpr SimpleArray& operator=(SimpleArray&&) = default;

    // Element access
    constexpr T& operator[](size_t index) noexcept
    {
        return storage[index];
    }

    constexpr const T& operator[](size_t index) const noexcept
    {
        return storage[index];
    }

    constexpr T& front() noexcept
    {
        return storage[0];
    }

    constexpr const T& front() const noexcept
    {
        return storage[0];
    }

    constexpr T& back() noexcept
    {
        return storage[N - 1];
    }

    constexpr const T& back() const noexcept
    {
        return storage[N - 1];
    }

    constexpr T* data() noexcept
    {
        return storage;
    }

    constexpr const T* data() const noexcept
    {
        return storage;
    }

    // Capacity
    static constexpr size_t size() noexcept
    {
        return N;
    }

    static constexpr bool empty() noexcept
    {
        return N == 0;
    }

    // Iterators
    constexpr T* begin() noexcept
    {
        return storage;
    }

    constexpr const T* begin() const noexcept
    {
        return storage;
    }

    constexpr T* end() noexcept
    {
        return storage + N;
    }

    constexpr const T* end() const noexcept
    {
        return storage + N;
    }

    constexpr void fill(const T& value)
    {
        for (size_t i = 0; i < N; ++i)
            storage[i] = value;
    }

    constexpr void swap(SimpleArray& other)
    {
        for (size_t i = 0; i < N; ++i)
        {
            T tmp = storage[i];
            storage[i] = other.storage[i];
            other.storage[i] = tmp;
        }
    }
};
} // utils
#endif
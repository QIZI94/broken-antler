#ifndef ORDERED_ENUMERATED_ARRAY
#define ORDERED_ENUMERATED_ARRAY
#include "simplearray.h"

#include "typehelpers.h"
namespace utils{

namespace detail
{
    template<typename T>
    constexpr void swap(T& a, T& b)
    {
        T tmp = a;
        a = b;
        b = tmp;
    }


    template<typename T, size_t N, typename Compare>
    constexpr void insertionSort(T (&array)[N], Compare compare)
    {
        for (size_t i = 1; i < N; ++i)
        {
            size_t j = i;

            while (j > 0 && compare(array[j], array[j - 1]))
            {
                swap(array[j], array[j - 1]);
                --j;
            }
        }
    }
}

template<typename K, typename V, size_t N>
constexpr SimpleArray<V, N>
MakeOrderedArrayFromEnumeratedArray(const KeyValuePair<K, V> (&input)[N])
{
    SimpleArray<KeyValuePair<K, V>, N> sorted{};

    for (size_t i = 0; i < N; ++i)
        sorted[i] = input[i];

    detail::insertionSort(
        sorted.storage,
        [](const auto& a, const auto& b)
        {
            return a.key < b.key;
        }
    );

    SimpleArray<V, N> result{};

    for (size_t i = 0; i < N; ++i)
        result[i] = sorted[i].value;

    return result;
}
}

#endif
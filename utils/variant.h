#ifndef SIMPLE_VARIANT_H
#define SIMPLE_VARIANT_H


#include <new>     // placement new
#include <stdint.h>

using size_t = decltype(sizeof(0));

//------------------------------------------------------------
// Placement new
//------------------------------------------------------------



//------------------------------------------------------------
// Compile-time utilities
//------------------------------------------------------------

template<size_t A, size_t B>
struct StaticMax
{
    static constexpr size_t value = (A > B) ? A : B;
};

template<typename T, typename... Rest>
struct MaxSize
{
    static constexpr size_t value =
        StaticMax<sizeof(T), MaxSize<Rest...>::value>::value;
};

template<typename T>
struct MaxSize<T>
{
    static constexpr size_t value = sizeof(T);
};

template<typename T, typename... Rest>
struct MaxAlign
{
    static constexpr size_t value =
        StaticMax<alignof(T), MaxAlign<Rest...>::value>::value;
};

template<typename T>
struct MaxAlign<T>
{
    static constexpr size_t value = alignof(T);
};

//------------------------------------------------------------
// Type index
//------------------------------------------------------------

template<typename T, typename... Ts>
struct TypeIndex;

template<typename T, typename... Rest>
struct TypeIndex<T, T, Rest...>
{
    static constexpr unsigned char value = 0;
};

template<typename T, typename First, typename... Rest>
struct TypeIndex<T, First, Rest...>
{
    static constexpr unsigned char value =
        1 + TypeIndex<T, Rest...>::value;
};

template<typename T>
struct TypeIndex<T>
{
    static_assert(sizeof(T) == 0, "Type not found in Variant");
};

//------------------------------------------------------------
// Destroy helper
//------------------------------------------------------------

template<unsigned char I, typename... Ts>
struct DestroyHelper;

template<unsigned char I, typename T, typename... Rest>
struct DestroyHelper<I, T, Rest...>
{
    static void destroy(unsigned char index, void* storage)
    {
        if(index == I)
        {
            reinterpret_cast<T*>(storage)->~T();
        }
        else
        {
            DestroyHelper<I + 1, Rest...>::destroy(index, storage);
        }
    }
};

template<unsigned char I>
struct DestroyHelper<I>
{
    static void destroy(unsigned char, void*)
    {
    }
};

//------------------------------------------------------------
// Variant
//------------------------------------------------------------

template<typename... Ts>
class Variant
{
    alignas(MaxAlign<Ts...>::value)
    unsigned char storage[MaxSize<Ts...>::value];

    unsigned char typeIndex = 0xFF;

public: // definitions
	template<typename T>
    struct IndexOf
    {
        static constexpr unsigned char value = TypeIndex<T, Ts...>::value;
    };
public: // static functions
	template<typename T>
	static constexpr unsigned char indexOf(){
		return IndexOf<T>::value;
	}
	static constexpr unsigned char idexOfNone(){
		return 0xFF;
	}
public: // member functions

    Variant() = default;

    ~Variant()
    {
        reset();
    }

    Variant(const Variant&) = delete;
    Variant& operator=(const Variant&) = delete;

    void reset()
    {
        if(typeIndex != 0xFF)
        {
            DestroyHelper<0, Ts...>::destroy(typeIndex, storage);
            typeIndex = 0xFF;
        }
    }

    template<typename T>
    void emplace(const T& value)
    {
        reset();

        new(storage) T(value);

        typeIndex = TypeIndex<T, Ts...>::value;
    }

    template<typename T, typename... Args>
    void emplace(Args&&... args)
    {
        reset();

        new(storage) T(static_cast<Args&&>(args)...);

        typeIndex = TypeIndex<T, Ts...>::value;
    }

	template<typename T>
	Variant& operator=(const T& value)
	{
		emplace<T>(value);
		return *this;
	}

    template<typename T>
    bool is() const
    {
        return typeIndex == TypeIndex<T, Ts...>::value;
    }

    template<typename T>
    T& get()
    {
        return *reinterpret_cast<T*>(storage);
    }
	

    template<typename T>
    const T& get() const
    {
        return *reinterpret_cast<const T*>(storage);
    }
	template<typename T>
	T* try_get(){
		return is<T>() ? &get<T>() : nullptr;
	}
	template<typename T>
	const T* try_get() const {
		return is<T>() ? &get<T>() : nullptr;
	}

    bool valid() const
    {
        return typeIndex != 0xFF;
    }

    unsigned char index() const
    {
        return typeIndex;
    }

	operator unsigned char() const {
		return typeIndex;
	}
};
#endif 
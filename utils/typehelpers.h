#ifndef TYPE_HELPERS_H
#define TYPE_HELPERS_H
#include <inttypes.h>
#include <stddef.h>
namespace utils{
struct PhantomData{};

template<typename T>
union ForceNoInit{
	constexpr ForceNoInit() : phantom(PhantomData{}) {}
	PhantomData phantom;
	T value;
	constexpr operator T(){return value;}
};

template<size_t N, int Tag>
struct FittingUnsignedIntImpl;

template<size_t N>
struct FittingUnsignedInt {
    static constexpr int tag =
        (N <= UINT8_MAX) ? 0 :
        (N <= UINT16_MAX) ? 1 :
        (N <= UINT32_MAX)  ? 2 : 3;

    using type = typename FittingUnsignedIntImpl<N, tag>::type;
};

/* specializations */
template<size_t N>
struct FittingUnsignedIntImpl<N, 0> { using type = uint8_t; };

template<size_t N>
struct FittingUnsignedIntImpl<N, 1> { using type = uint16_t; };

template<size_t N>
struct FittingUnsignedIntImpl<N, 2> { using type = uint32_t; };

template<size_t N>
struct FittingUnsignedIntImpl<N, 3> { using type = uint64_t; };
template<typename T>
struct NumericLimits{
	
	static constexpr T (max)(){
		return T(-1);
	}
};

template<typename T>
struct remove_const
{
    using type = T;
};

template<typename T>
struct remove_const<const T>
{
    using type = T;
};

template<typename From, typename To>
struct copy_const
{
    using type = To;
};

template<typename From, typename To>
struct copy_const<const From, To>
{
    using type = const To;
};

template<bool B, typename T = void>
struct enable_if {};

template<typename T>
struct enable_if<true, T>
{
    using type = T;
};

template<bool B, typename T = void>
using enable_if_t = typename enable_if<B, T>::type;

template<unsigned... I>
struct index_sequence {};

template<unsigned N, unsigned... I>
struct make_index_sequence
{
    using type = typename make_index_sequence<N - 1, N - 1, I...>::type;
};

template<unsigned... I>
struct make_index_sequence<0, I...>
{
    using type = index_sequence<I...>;
};

template<unsigned N>
using make_index_sequence_t = typename make_index_sequence<N>::type;

template<typename K, typename V>
struct KeyValuePair{
	K key;
	V value;
};
template<typename K, typename V>
inline constexpr KeyValuePair<K, V> MakeKeyValuePair(const K& key, const V& value){
	return KeyValuePair<K, V>{.key = key, .value = value};
}

template<typename Callable>
class Deferred
{
public:
    constexpr Deferred(Callable callable)
        : callable(callable)
    {
    }

    ~Deferred()
    {
        callable();
    }

    Deferred(const Deferred&) = delete;
    Deferred& operator=(const Deferred&) = delete;

private:
    Callable callable;
};
template<typename Callable>
Deferred(Callable) -> Deferred<Callable>;

template<size_t N, typename T>
struct InitializedArrayWrapper
{
    T data[N];

    template<unsigned... I>
    constexpr InitializedArrayWrapper(T value, index_sequence<I...>)
        : data{ ((void)I, value)... }
    {}
    
    constexpr InitializedArrayWrapper(T value)
        : InitializedArrayWrapper(value, make_index_sequence_t<N>{})
    {}
	constexpr T& operator[](size_t i) { return data[i]; }
    constexpr const T& operator[](size_t i) const { return data[i]; }
	static constexpr typename FittingUnsignedInt<N>::type size() { return N;}
};

inline constexpr const size_t PlatformAlignment = alignof(size_t);
	
} // utils


#define _DEFINE_HAS_METHOD_IMPLEMENTED(name)                         \
template <typename T, typename Sig>                           \
struct has_##name##_implemented;                                    \
                                                              \
template <typename T, typename R, typename... Args>           \
struct has_##name##_implemented<T, R (T::*)(Args...)> {              \
    typedef char yes[1];                                       \
    typedef char no[2];                                        \
                                                              \
    template <typename U>                                      \
    static yes& test(                                          \
        decltype(static_cast<R (U::*)(Args...)>(&U::name))*); \
                                                              \
    template <typename>                                        \
    static no& test(...);                                      \
                                                              \
    static constexpr bool value =                              \
        sizeof(test<T>(0)) == sizeof(yes);                    \
};                                                            \
                                                              \
template <typename T, typename R, typename... Args>           \
struct has_##name##_implemented<T, R (T::*)(Args...) const> {        \
    typedef char yes[1];                                       \
    typedef char no[2];                                        \
                                                              \
    template <typename U>                                      \
    static yes& test(                                          \
        decltype(static_cast<R (U::*)(Args...) const>(&U::name))*); \
                                                              \
    template <typename>                                        \
    static no& test(...);                                      \
                                                              \
    static constexpr bool value =                              \
        sizeof(test<T>(0)) == sizeof(yes);                    \
};

#define _HAS_METHOD_IMPLEMENTED_HELPER(DerivedClass, Method, ReturnType, Args) \
static_assert(\
	has_##Method##_implemented<DerivedClass, ReturnType (DerivedClass::*)Args>::value,\
	"Derived class doesn't implement method with signature '" #ReturnType " " #Method #Args\
);

#endif // TYPE_HELPERS_H
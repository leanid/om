#include <cstdlib>
#include <functional>
#include <iostream>
#include <set>

struct my_type
{
    int  i;
    bool operator<(const my_type& other) const { return i < other.i; }
};

template <class T> class MyAllocator
{
public:
    using value_type                             = T;
    using size_type                              = std::size_t;
    using difference_type                        = std::ptrdiff_t;
    using propagate_on_container_move_assignment = std::true_type;

    T* allocate(size_type n, const void* hint = 0)
    {
        std::cout << "Alloc" << n << std::endl;
        return static_cast<T*>(malloc(n * sizeof(T)));
    }

    void      deallocate(T* p, size_type n) { free(p); }
    size_type max_size() const
    {
        return size_type(std::numeric_limits<unsigned int>::max() / sizeof(T));
    }
    void construct(T* p, const T& value) { _construct(p, value); }
    void destroy(T* p) { _destroy(p); }
};

template <class T, class U>
bool operator==(const MyAllocator<T>&, const MyAllocator<U>&) noexcept
{
    return true;
}

template <class T, class U>
bool operator!=(const MyAllocator<T>&, const MyAllocator<U>&) noexcept
{
    return false;
}

// clang++ -std=c++20 -stdlib=libc++ -DFIX_LIBCXX ./main.cxx
// clang++ -std=c++20 -stdlib=libc++ ./main.cxx
#if defined(FIX_LIBCXX) // 1 to fix error
namespace std
{
// we need three template parameters to find this if using custom Allocator
template <class Key, class Comparator, class Allocator>
auto operator<=>(const set<Key, Comparator, Allocator>& l,
                 const set<Key, Comparator, Allocator>& r)
{
    return std::lexicographical_compare_three_way(
        l.begin(), l.end(), r.begin(), r.end(), std::__synth_three_way);
}
} // namespace std
#endif

int main()
{
    std::set<my_type> s01;
    std::set<my_type> s02;
    std::cout << "(s01 < s02): " << (s01 < s02) << std::endl; // compiles

    std::set<my_type, std::less<my_type>> s11;
    std::set<my_type, std::less<my_type>> s12;

    std::cout << "(s11 < s12): " << (s11 < s12) << std::endl; // compiles

    std::set<my_type, std::less<my_type>, std::allocator<my_type>> s21;
    std::set<my_type, std::less<my_type>, std::allocator<my_type>> s22;

    std::cout << "(s21 < s22): " << (s21 < s22) << std::endl; // compiles

    std::set<my_type, std::less<my_type>, MyAllocator<my_type>> s31;
    std::set<my_type, std::less<my_type>, MyAllocator<my_type>> s32;

    std::cout << "(s31 < s32): " << (s31 < s32) << std::endl; // error
    return EXIT_SUCCESS;
}

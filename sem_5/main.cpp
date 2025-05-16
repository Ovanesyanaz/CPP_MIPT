#include <iostream>

unsigned long long fact(unsigned long n){
    if (n == 0 || n == 1){
        return 1;
    }
    return n * fact(n - 1);
}

namespace detail {
    template <unsigned long N>
    struct factorial_impl {
        static constexpr unsigned long long value = N * factorial_impl<N - 1>::value;
    };

    template <>
    struct factorial_impl<0> {
        static constexpr unsigned long long value = 1;
    };
};

template <unsigned long N>
constexpr unsigned long long factorial = detail::factorial_impl<N>::value;

int main(int argc, char const *argv[])
{
    std::cout << fact(17) << std::endl;
    std::cout << factorial<17> << std::endl;
}

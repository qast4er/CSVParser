#pragma once
#include <iostream>
#include <tuple>
//#include <utility>


//using my__index sequence and recursive templates
namespace aux {
    template<std::size_t...>
    struct sequence {};

    template<std::size_t N, std::size_t ...Is>
    struct generate_sequence : generate_sequence<N - 1, N - 1, Is...> {};

    template<std::size_t ...Is>
    struct generate_sequence<0, Is...> : sequence<Is...> {};

    template<class Ch, class Tr, class Tuple, std::size_t ...Is>
    void print_tuple(std::basic_ostream<Ch, Tr>& os, Tuple const& t, sequence<Is...>) {
        using swallow = int[];
        swallow { 0, (void(os << (Is == 0 ? "" : ", ") << std::get<Is>(t)), 0)... };
    }
} 

template<class Ch, class Tr, class ...Args>
auto& operator<<(std::basic_ostream<Ch, Tr>& os, std::tuple<Args...> const& t) {
    aux::print_tuple(os, t, aux::generate_sequence<sizeof...(Args)>{});
    return os;
}


/*
//using std :: index_sequence_for and recursive templates in it
template<class Ch, class Tr, class Tuple, std::size_t... Is>
void print_tuple_impl(std::basic_ostream<Ch, Tr>& os, Tuple const& t, std::index_sequence<Is...>) {
    //((os << (Is == 0 ? "" : ", ") << std::get<Is>(t)), ...);
    using swallow = int[];
    (void)swallow {
        0, (void(os << (Is == 0 ? "" : ", ") << std::get<Is>(t)), 0)...
    };
}

template<class Ch, class Tr, class... Args>
auto& operator<<(std::basic_ostream<Ch, Tr>& os, std::tuple<Args...>const& t) {
    //os << "(";
    print_tuple_impl(os, t, std::index_sequence_for<Args...>{});
    return os; //<< ")";
}*/


//since C++23
/*template<class Ch, class Tr, typename ...Args>
auto& operator<<(std::basic_ostream<Ch, Tr>& os, std::tuple<Args...> const& t) {
    std::apply
    (
        [&os](Args const&... tupleArgs)
        {
            os << '[';
            std::size_t n{ 0 };
            ((os << tupleArgs << (++n != sizeof...(Args) ? ", " : "")), ...);
            os << ']';
        }, t
    );
    return os;
}*/
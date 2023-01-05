#include <memory>
#include <ostream>
#include <sstream>
#include <tuple>
#include <vector>

template<class ...Args>
using tuple_ptr = std::unique_ptr<std::tuple<Args...>>;

namespace {
template <size_t first, size_t last, size_t size, class _Callback, class ...Args>
class Iteration {
public:
    static void next(_Callback callback, std::tuple<Args...> t) {
        callback(std::get<first>(t), false);
        Iteration<first + 1, last, size, _Callback, Args...>::next(callback, t);
    }

    static void next(_Callback callback, tuple_ptr<Args...>& t_p) {
        callback(std::get<first>(*t_p), false);
        Iteration<first + 1, last, size, _Callback, Args...>::next(callback, t_p);
    }
};

// last iteration
template <size_t last, size_t size, class _Callback, class ...Args>
class Iteration<last, last, size, _Callback, Args...> {
public:
    static void next(_Callback callback, std::tuple<Args...> t) {
        callback(std::get<last>(t), true);
    }

    static void next(_Callback callback, tuple_ptr<Args...>& t_p) {
        callback(std::get<last>(*t_p), true);
    }
};

// empty tuple
template <size_t first, size_t last, class _Callback, class ...Args>
class Iteration<first, last, 0, _Callback, Args...> {
public:
    static void next(_Callback callback, std::tuple<Args...> t) {}
    static void next(_Callback callback, tuple_ptr<Args...>& t_p) {}
};

template <class _Callback, class ...Args>
void for_each(_Callback callback, std::tuple<Args...> t) {
    Iteration<0, sizeof...(Args) - 1, sizeof...(Args), _Callback, Args...>::next(callback, t);
}

template <class _Callback, class ...Args>
void for_each(_Callback callback, tuple_ptr<Args...>& t_p) {
    Iteration<0, sizeof...(Args) - 1, sizeof...(Args), _Callback, Args...>::next(callback, t_p);
}

template<class Ch, class Tr>
class PrintCallback {
public:
    explicit PrintCallback(std::basic_ostream<Ch, Tr>& os) : CSV_os(os) {}

    template<class T>
    void operator()(T value, bool is_last) {
        CSV_os << value;
        if (!is_last) {
            CSV_os << "; ";
        }
    }

private:
    std::basic_ostream<Ch, Tr>& CSV_os;
};

class MakeCallback {
public:
    explicit MakeCallback(std::vector<std::string>& values) : CSV_values(values), CSV_cur_idx(0) {}

    template<class T>
    void operator()(T& value, bool is_last) {
        std::istringstream iss(CSV_values[CSV_cur_idx]);
        iss >> value;

        std::ostringstream oss;
        oss << value;

        if (oss.str() != CSV_values[CSV_cur_idx]) {
            throw CSV_cur_idx;
        }

        CSV_cur_idx++;
    }

    // specialization for std::string
    void operator()(std::string& value, bool is_last) {
        value = CSV_values[CSV_cur_idx];
        CSV_cur_idx++;
    }

private:
    std::vector<std::string> CSV_values;
    size_t CSV_cur_idx;
};
}

template<class Ch, class Tr, class ...Args>
auto& operator<<(std::basic_ostream<Ch, Tr>& os, const std::tuple<Args...>& t) {
    PrintCallback<Ch, Tr> print_callback(os);
    for_each(print_callback, t);
    return os;
}

template<class ...Args>
std::tuple<Args...> make_tuple(std::vector<std::string> values) {
    auto t_p = std::make_unique<std::tuple<Args...>>();
    MakeCallback make_callback(values);
    for_each(make_callback, t_p);
    return *t_p;
}
#pragma once
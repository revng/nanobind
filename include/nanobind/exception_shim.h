#pragma once

#include <cstdio>
#include <cstdlib>

#define throwShim(X) shim::throw_(X, __FILE__, __LINE__)

namespace shim {

template<typename T>
[[noreturn]] inline void throw_(T &&Exception, const char *Filename, unsigned Line) {
    fprintf(stderr, "Exception thrown at %s:%d\n%s\n", Filename, Line, Exception.what());
    std::abort();
}

class exception_placeholder {
public:
    exception_placeholder() = default;
    ~exception_placeholder() = default;

    template<typename T>
    exception_placeholder& operator=(const T &) { std::abort(); }
    void restore() { std::abort(); }
    const char *what() { return ""; }
};

}

NAMESPACE_BEGIN(NB_NAMESPACE)
NAMESPACE_BEGIN(detail)

inline bool set_builtin_exception_status(shim::exception_placeholder &) {
    std::abort();
}

NAMESPACE_END(detail)
NAMESPACE_END(NB_NAMESPACE)

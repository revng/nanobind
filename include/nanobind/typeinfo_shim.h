#pragma once

#include <cstdlib>
#include <cassert>
#include <string>

namespace shim {

class type_info {
private:
    const char *prettyName = nullptr;

public:
    type_info(const char *prettyName) : prettyName(prettyName){};

    /// Return the stringified name of the type. This should be similar enough
    /// with what std::type_info does.
    const char *name() const { return prettyName; };

    bool operator==(const shim::type_info &Other) const {
        // This works because TypeStorage below will return the same
        // `type_info` object for the same type
        return this == &Other;
    }
};

}

template<typename T>
class TypeStorage {
private:
    /// This function will return the name of T, statically
    /// It leverages the `__PRETTY_FUNCTION__` compiler macro which expands
    /// template paramters to their class name
    static inline const char *_name() {
        // Store the macro in a string, this will be in the form
        // <some text>TypeStorage<$TYPE_NAME>::_name()<some text>
        std::string UglyName{__PRETTY_FUNCTION__};

        // Find the position where the tail end of the string we're
        //interested in
        std::string EndMarker = ">::_name()";
        size_t EndPos = UglyName.rfind(EndMarker);
        assert(EndPos > 1);

        // Iterate backwards through the string, counting the number of '>' and
        // '<'s encountered, this allows this function to work in cases where T
        // has template parameters, e.g. TypeStorage<Foo<Bar>>
        size_t StartPos = 0;
        size_t Counter = 0;
        for (size_t I = EndPos - 1; I >= 0; I--) {
            // If here it means we balanced out the outermost > with a <,
            // save the position and break
            if (UglyName[I] == '<' and Counter == 0) {
                StartPos = I + 1;
                break;
            }

            if (UglyName[I] == '>')
                Counter++;
            if (UglyName[I] == '<')
                Counter--;
        }

        assert(StartPos >= 0);
        assert(EndPos > StartPos);

        // Extract the substring and save it in static storage, this is needed
        // because std::type_info returns a `const char *` for `name`
        static std::string Name = UglyName.substr(StartPos, EndPos - StartPos);
        return Name.c_str();
    }

public:
    static inline shim::type_info ID{_name()};
};

template<typename T>
shim::type_info &typeidShim() {
    // Return a std::type_info-like object. This will always be the same object
    // given the same type. This works be leveraging WEAK library symbols. It
    // will not work in some cases (e.g. `dlopen(..., RTLD_LOCAL)`)
    return TypeStorage<T>::ID;
}

shim::type_info &typeidShim(auto) {
    // This is the typeidShim for values, since we don't have RTTI the
    // shim::type_info cannot be retrieved.
    std::abort();
}

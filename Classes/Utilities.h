#ifndef __UTILITIES_H__
#define __UTILITIES_H__

#include <string>
#include <sstream>

namespace wreckingmadness {
    class Utilities {
    public:
        template <typename T>
        static std::string to_string(T value);
    };

    template <typename T>
    std::string Utilities::to_string(T value) {
        std::ostringstream os;
        os << value;
        return os.str();
    }
}

#endif
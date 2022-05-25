#ifndef __UTILITIES_H__
#define __UTILITIES_H__

#include <string>
#include <sstream>
#include "cocos2d.h"

namespace wreckingmadness {
    enum ToastDuration {
        SHORT,
        MEDIUM,
        LONG
    };
    
    class Utilities {
    private:
        static std::random_device randomDevice;
        static std::mt19937 randomEngine;
        static std::uniform_real_distribution<float> floatDistribution;
        static std::map<ToastDuration, float> durationMapping;
    public:
        template <typename T>
        static std::string to_string(T value);
        static void makeToast(std::string const& text, ToastDuration duration);
        static float getRandom();
        static int getRandom(int min, int max);
        template <typename ...Args>
        static std::string formatString(const std::string& format, Args && ...args);
    };

    template <typename T>
    std::string Utilities::to_string(T value) {
        std::ostringstream os;
        os << value;
        return os.str();
    }

    template <typename ...Args>
    std::string Utilities::formatString(const std::string& format, Args && ...args)
    {
        auto size = std::snprintf(nullptr, 0, format.c_str(), std::forward<Args>(args)...);
        std::string output(size + 1, '\0');
        std::sprintf(&output[0], format.c_str(), std::forward<Args>(args)...);
        return output;
    }
}

#endif
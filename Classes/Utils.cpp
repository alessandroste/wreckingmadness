#include "Utils.h"

USING_NS_CC;

// for Android NDK
template <typename T>
std::string to_string(T value) {
	std::ostringstream os;
	os << value;
	return os.str();
}

std::string Utilities::floatToString(float num)
{
	return std::string();
}

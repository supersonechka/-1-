#pragma once
#include <string>
#include <vector>

class BinaryUtils {
public:
    static std::string binaryToSafeString(const std::vector<unsigned char>& data);
    static std::vector<unsigned char> safeStringToBinary(const std::string& str);
    static bool isSafeString(const std::string& str);
};

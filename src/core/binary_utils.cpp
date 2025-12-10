#include "core/binary_utils.h"
#include <sstream>
#include <iomanip>
#include <cctype>

std::string BinaryUtils::binaryToSafeString(const std::vector<unsigned char>& data) {
    std::stringstream ss;
    for (size_t i = 0; i < data.size(); i++) {
        ss << static_cast<int>(data[i]);
        if (i != data.size() - 1) {
            ss << " ";
        }
    }
    return ss.str();
}

std::vector<unsigned char> BinaryUtils::safeStringToBinary(const std::string& str) {
    std::vector<unsigned char> result;
    std::stringstream ss(str);
    int value;
    
    while (ss >> value) {
        if (value >= 0 && value <= 255) {
            result.push_back(static_cast<unsigned char>(value));
        }
    }
    
    return result;
}

bool BinaryUtils::isSafeString(const std::string& str) {
    if (str.empty()) return false;
    
    std::stringstream ss(str);
    int value;
    int count = 0;
    
    // Проверяем, что все токены - числа от 0 до 255
    while (ss >> value) {
        if (value < 0 || value > 255) {
            return false;
        }
        count++;
    }
    
    // Должен быть хотя бы один валидный номер
    return count > 0 && ss.eof();
}

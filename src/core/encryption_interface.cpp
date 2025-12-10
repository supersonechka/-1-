#include "core/encryption_interface.h"
#include <fstream>
#include <stdexcept>

std::vector<unsigned char> EncryptionInterface::readFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Cannot open file: " + filename);
    }
    
    file.seekg(0, std::ios::end);
    size_t size = file.tellg();
    file.seekg(0, std::ios::beg);
    
    std::vector<unsigned char> data(size);
    if (size > 0) {
        file.read(reinterpret_cast<char*>(data.data()), size);
    }
    return data;
}

bool EncryptionInterface::writeFile(const std::string& filename, const std::vector<unsigned char>& data) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        return false;
    }
    
    if (!data.empty()) {
        file.write(reinterpret_cast<const char*>(data.data()), data.size());
    }
    return file.good();
}

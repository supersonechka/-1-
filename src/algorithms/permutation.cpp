#include "algorithms/permutation.h"
#include <sstream>
#include <algorithm>
#include <iostream>
#include <vector>
#include <random>
#include <cstring>

bool PermutationCipher::encrypt(const std::string& inputFile, const std::string& outputFile,
                               const std::string& key) {
    try {
        auto data = EncryptionInterface::readFile(inputFile);
        auto permutation = parseKey(key);
        size_t blockSize = permutation.size();
        
        if (blockSize == 0) {
            std::cerr << "ключ не должен быть пустым" << std::endl;
            return false;
        }

        size_t padding = blockSize - (data.size() % blockSize);
        if (padding != blockSize) {
            data.insert(data.end(), padding, 0);
        }

        for (size_t i = 0; i < data.size(); i += blockSize) {
            std::vector<unsigned char> block(blockSize);
            
            for (size_t j = 0; j < blockSize; j++) {
                block[j] = data[i + permutation[j]];
            }
            
            for (size_t j = 0; j < blockSize; j++) {
                data[i + j] = block[j];
            }
        }

        return EncryptionInterface::writeFile(outputFile, data);
    } catch (const std::exception& e) {
        std::cerr << "ошибка шифрования: " << e.what() << std::endl;
        return false;
    }
}

bool PermutationCipher::decrypt(const std::string& inputFile, const std::string& outputFile,
                               const std::string& key) {
    try {
        auto data = EncryptionInterface::readFile(inputFile);
        auto permutation = parseKey(key);
        auto inversePermutation = invertPermutation(permutation);
        size_t blockSize = permutation.size();
        
        if (blockSize == 0) {
            std::cerr << "ключ не должен быть пустым." << std::endl;
            return false;
        }

        for (size_t i = 0; i < data.size(); i += blockSize) {
            std::vector<unsigned char> block(blockSize);
            
            for (size_t j = 0; j < blockSize; j++) {
                block[j] = data[i + inversePermutation[j]];
            }
            
            for (size_t j = 0; j < blockSize; j++) {
                data[i + j] = block[j];
            }
        }

        while (!data.empty() && data.back() == 0) {
            data.pop_back();
        }

        return EncryptionInterface::writeFile(outputFile, data);
    } catch (const std::exception& e) {
        std::cerr << "ошибка дешифрования: " << e.what() << std::endl;
        return false;
    }
}

std::string PermutationCipher::encryptText(const std::string& text, const std::string& key) {
    try {
        auto permutation = parseKey(key);
        size_t blockSize = permutation.size();
        
        if (blockSize == 0) {
            throw std::runtime_error("ключ не должен быть пустым");
        }

        std::vector<unsigned char> data(text.begin(), text.end());
        
        size_t padding = blockSize - (data.size() % blockSize);
        if (padding != blockSize) {
            data.insert(data.end(), padding, ' ');
        }

        for (size_t i = 0; i < data.size(); i += blockSize) {
            std::vector<unsigned char> block(blockSize);
            
            for (size_t j = 0; j < blockSize; j++) {
                block[j] = data[i + permutation[j]];
            }
            
            for (size_t j = 0; j < blockSize; j++) {
                data[i + j] = block[j];
            }
        }

        return std::string(data.begin(), data.end());
        
    } catch (const std::exception& e) {
        throw std::runtime_error("ошибка шифрования текста: " + std::string(e.what()));
    }
}

std::string PermutationCipher::decryptText(const std::string& text, const std::string& key) {
    try {
        auto permutation = parseKey(key);
        auto inversePermutation = invertPermutation(permutation);
        size_t blockSize = permutation.size();
        
        if (blockSize == 0) {
            throw std::runtime_error("ключ не должен быть пустым");
        }

        std::vector<unsigned char> data(text.begin(), text.end());

        for (size_t i = 0; i < data.size(); i += blockSize) {
            std::vector<unsigned char> block(blockSize);
            
            for (size_t j = 0; j < blockSize; j++) {
                block[j] = data[i + inversePermutation[j]];
            }
            
            for (size_t j = 0; j < blockSize; j++) {
                data[i + j] = block[j];
            }
        }

        while (!data.empty() && data.back() == ' ') {
            data.pop_back();
        }

        return std::string(data.begin(), data.end());
        
    } catch (const std::exception& e) {
        throw std::runtime_error("ошибка дешифровки текста: " + std::string(e.what()));
    }
}

std::vector<size_t> PermutationCipher::parseKey(const std::string& key) {
    std::vector<size_t> result;
    std::istringstream iss(key);
    std::string token;
    
    while (std::getline(iss, token, ',')) {
        try {
            token.erase(0, token.find_first_not_of(" \t"));
            token.erase(token.find_last_not_of(" \t") + 1);
            
            if (token.empty()) {
                continue;
            }
            
            result.push_back(std::stoul(token));
        } catch (...) {
            throw std::runtime_error("неправильный вид ключа");
        }
    }
    
    if (result.empty()) {
        throw std::runtime_error("ключ не должен быть пустым");
    }
    
    std::vector<bool> seen(result.size(), false);
    for (size_t i : result) {
        if (i >= result.size()) {
            throw std::runtime_error("ошибка " +
                                   std::to_string(result.size()) + ",используются значения с 0 " +
                                   std::to_string(result.size()-1));
        }
        if (seen[i]) {
            throw std::runtime_error("ошибка! все числа должны быть уникальными");
        }
        seen[i] = true;
    }
    
    return result;
}

std::vector<size_t> PermutationCipher::invertPermutation(const std::vector<size_t>& permutation) {
    std::vector<size_t> inverse(permutation.size());
    for (size_t i = 0; i < permutation.size(); i++) {
        inverse[permutation[i]] = i;
    }
    return inverse;
}

std::string PermutationCipher::generateKey() {
    return "1,2,0";
}

extern "C" {
    void* createAlgorithm() {
        return new PermutationCipher();
    }
    
    bool encrypt(void* algorithm, const char* inputFile, const char* outputFile, const char* key) {
        PermutationCipher* cipher = static_cast<PermutationCipher*>(algorithm);
        return cipher->encrypt(inputFile, outputFile, key);
    }
    
    bool decrypt(void* algorithm, const char* inputFile, const char* outputFile, const char* key) {
        PermutationCipher* cipher = static_cast<PermutationCipher*>(algorithm);
        return cipher->decrypt(inputFile, outputFile, key);
    }
    
    const char* encryptText(void* algorithm, const char* text, const char* key) {
        PermutationCipher* cipher = static_cast<PermutationCipher*>(algorithm);
        std::string result = cipher->encryptText(text, key);
        char* c_str = new char[result.length() + 1];
        std::strcpy(c_str, result.c_str());
        return c_str;
    }
    
    const char* decryptText(void* algorithm, const char* text, const char* key) {
        PermutationCipher* cipher = static_cast<PermutationCipher*>(algorithm);
        std::string result = cipher->decryptText(text, key);
        char* c_str = new char[result.length() + 1];
        std::strcpy(c_str, result.c_str());
        return c_str;
    }
    
    const char* generateKey(void* algorithm) {
        PermutationCipher* cipher = static_cast<PermutationCipher*>(algorithm);
        std::string result = cipher->generateKey();
        char* c_str = new char[result.length() + 1];
        std::strcpy(c_str, result.c_str());
        return c_str;
    }
    
    void freeString(const char* str) {
        delete[] str;
    }
    
    void destroyAlgorithm(void* algorithm) {
        delete static_cast<PermutationCipher*>(algorithm);
    }
}

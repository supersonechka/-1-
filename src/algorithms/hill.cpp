#include "algorithms/hill.h"
#include <sstream>
#include <stdexcept>
#include <iostream>
#include <random>
#include <vector>
#include <cstring>
#include <locale>
#include <codecvt>

bool HillCipher::encrypt(const std::string& inputFile, const std::string& outputFile,
                        const std::string& key) {
    try {
        auto data = EncryptionInterface::readFile(inputFile);
        auto keyMatrix = parseKey(key);
        processData(data, keyMatrix, true);
        return EncryptionInterface::writeFile(outputFile, data);
    } catch (const std::exception& e) {
        std::cerr << "Hill Encryption error: " << e.what() << std::endl;
        return false;
    }
}
bool HillCipher::decrypt(const std::string& inputFile, const std::string& outputFile,
                        const std::string& key) {
    try {
        auto data = EncryptionInterface::readFile(inputFile);
        auto keyMatrix = parseKey(key);
        auto inverseMatrix = getInverseMatrix(keyMatrix);
        processData(data, inverseMatrix, false);
        return EncryptionInterface::writeFile(outputFile, data);
    } catch (const std::exception& e) {
        std::cerr << "Ошибка дешифровки: " << e.what() << std::endl;
        return false;
    }
}
std::string HillCipher::encryptText(const std::string& text, const std::string& key) {
    try {
        auto keyMatrix = parseKey(key);
        std::vector<unsigned char> data(text.begin(), text.end());
        processData(data, keyMatrix, true);
        return std::string(data.begin(), data.end());
    } catch (const std::exception& e) {
        throw std::runtime_error("Ошибка шифрования текста: " + std::string(e.what()));
    }
}
std::string HillCipher::decryptText(const std::string& text, const std::string& key) {
    try {
        auto keyMatrix = parseKey(key);
        auto inverseMatrix = getInverseMatrix(keyMatrix);
        std::vector<unsigned char> data(text.begin(), text.end());
        processData(data, inverseMatrix, false);
        return std::string(data.begin(), data.end());
    } catch (const std::exception& e) {
        throw std::runtime_error("Ошибка дешифровки текста: " + std::string(e.what()));
    }
}
void HillCipher::processData(std::vector<unsigned char>& data,
                           const std::vector<int>& keyMatrix,
                           bool isEncrypt) {
    size_t originalSize = data.size();
    if (data.size() % 2 != 0) {
        data.push_back(0);
    }
    for (size_t i = 0; i < data.size(); i += 2) {
        int x1 = static_cast<unsigned char>(data[i]);
        int x2 = static_cast<unsigned char>(data[i + 1]);
        
        int y1 = mod256(keyMatrix[0] * x1 + keyMatrix[1] * x2);
        int y2 = mod256(keyMatrix[2] * x1 + keyMatrix[3] * x2);
        
        data[i] = static_cast<unsigned char>(y1);
        data[i + 1] = static_cast<unsigned char>(y2);
    }
    if (!isEncrypt && data.size() > originalSize) {
        data.resize(originalSize);
    }
}
std::vector<int> HillCipher::getInverseMatrix(const std::vector<int>& keyMatrix) {
    int det = mod256(keyMatrix[0] * keyMatrix[3] - keyMatrix[1] * keyMatrix[2]); //определитель
    
    int invDet = -1;
    for (int i = 1; i < 256; i++) {
        if (mod256(det * i) == 1) {
            invDet = i;
            break;
        }
    }
    if (invDet == -1) {
        throw std::runtime_error("матрица не вычисляема mod 256. детерминант: " + std::to_string(det));
    }
    std::vector<int> inverseMatrix(4);
    inverseMatrix[0] = mod256( keyMatrix[3] * invDet);
    inverseMatrix[1] = mod256(-keyMatrix[1] * invDet);
    inverseMatrix[2] = mod256(-keyMatrix[2] * invDet);
    inverseMatrix[3] = mod256( keyMatrix[0] * invDet);
    return inverseMatrix;
}
std::vector<int> HillCipher::parseKey(const std::string& key) {
    std::vector<int> result;
    std::istringstream iss(key);
    std::string token;
    while (std::getline(iss, token, ',')) {
        token.erase(0, token.find_first_not_of(" \t"));
        token.erase(token.find_last_not_of(" \t") + 1);
        if (!token.empty()) {
            try {
                result.push_back(std::stoi(token));
            } catch (const std::exception& e) {
                throw std::runtime_error("неправильное значение ключа: '" + token + "'");
            }
        }
    }
    if (result.size() != 4) {
        throw std::runtime_error("ключ для шифра Хилла должен содержать 4 значения: " + std::to_string(result.size()));
    }
    return result;
}
int HillCipher::mod256(int value) {
    int result = value % 256;
    if (result < 0) {
        result += 256;
    }
    return result;
}
std::string HillCipher::generateKey() {
    return "49,44,159,219";
}
extern "C" {
    void* createAlgorithm() {
        return new HillCipher();
    }
    bool encrypt(void* algorithm, const char* inputFile, const char* outputFile, const char* key) {
        HillCipher* cipher = static_cast<HillCipher*>(algorithm);
        return cipher->encrypt(inputFile, outputFile, key);
    }
    bool decrypt(void* algorithm, const char* inputFile, const char* outputFile, const char* key) {
        HillCipher* cipher = static_cast<HillCipher*>(algorithm);
        return cipher->decrypt(inputFile, outputFile, key);
    }
    const char* encryptText(void* algorithm, const char* text, const char* key) {
        HillCipher* cipher = static_cast<HillCipher*>(algorithm);
        std::string result = cipher->encryptText(text, key);
        char* c_str = new char[result.length() + 1];
        std::strcpy(c_str, result.c_str());
        return c_str;
    }
    const char* decryptText(void* algorithm, const char* text, const char* key) {
        HillCipher* cipher = static_cast<HillCipher*>(algorithm);
        std::string result = cipher->decryptText(text, key);
        char* c_str = new char[result.length() + 1];
        std::strcpy(c_str, result.c_str());
        return c_str;
    }
    const char* generateKey(void* algorithm) {
        HillCipher* cipher = static_cast<HillCipher*>(algorithm);
        std::string result = cipher->generateKey();
        char* c_str = new char[result.length() + 1];
        std::strcpy(c_str, result.c_str());
        return c_str;
    }
    void freeString(const char* str) {
        delete[] str;
    }
    void destroyAlgorithm(void* algorithm) {
        delete static_cast<HillCipher*>(algorithm);
    }
}

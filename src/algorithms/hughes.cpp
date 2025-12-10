#include "algorithms/hughes.h"
#include <sstream>
#include <random>
#include <numeric>
#include <algorithm>
#include <iostream>
#include <cstring>
#include <locale>
#include <codecvt>

bool HughesCipher::encrypt(const std::string& inputFile, const std::string& outputFile,
                          const std::string& key) {
    try {
        auto data = EncryptionInterface::readFile(inputFile);
        initializeRotors(key);
        
        for (size_t i = 0; i < data.size(); i++) {
            data[i] = processByte(data[i]);
        }
        
        return EncryptionInterface::writeFile(outputFile, data);
    } catch (const std::exception& e) {
        std::cerr << "Hughes Encryption error: " << e.what() << std::endl;
        return false;
    }
}

bool HughesCipher::decrypt(const std::string& inputFile, const std::string& outputFile,
                          const std::string& key) {
    try {
        auto data = EncryptionInterface::readFile(inputFile);
        initializeRotors(key);
        
        for (size_t i = 0; i < data.size(); i++) {
            data[i] = processByte(data[i]);
        }
        
        return EncryptionInterface::writeFile(outputFile, data);
    } catch (const std::exception& e) {
        std::cerr << "Hughes Decryption error: " << e.what() << std::endl;
        return false;
    }
}

std::string HughesCipher::encryptText(const std::string& text, const std::string& key) {
    try {
        initializeRotors(key);
        
        std::vector<unsigned char> data(text.begin(), text.end());
        std::vector<unsigned char> result;
        
        for (size_t i = 0; i < data.size(); i++) {
            unsigned char encrypted = processByte(data[i]);
            result.push_back(encrypted);
        }
        
        return std::string(result.begin(), result.end());
        
    } catch (const std::exception& e) {
        throw std::runtime_error("Hughes Text Encryption error: " + std::string(e.what()));
    }
}

std::string HughesCipher::decryptText(const std::string& text, const std::string& key) {
    try {
        initializeRotors(key);
        
        std::vector<unsigned char> data(text.begin(), text.end());
        std::vector<unsigned char> result;
        
        for (size_t i = 0; i < data.size(); i++) {
            unsigned char decrypted = processByte(data[i]);
            result.push_back(decrypted);
        }
        
        return std::string(result.begin(), result.end());
        
    } catch (const std::exception& e) {
        throw std::runtime_error("Hughes Text Decryption error: " + std::string(e.what()));
    }
}

unsigned char HughesCipher::processByte(unsigned char input) {
    int signal = input;
    
    // Проход вперед через роторы
    for (size_t i = 0; i < rotors.size(); i++) {
        int inputPos = (signal + rotors[i].position) % 256;
        signal = rotors[i].wiring[inputPos];
        signal = (signal - rotors[i].position + 256) % 256;
    }
    
    // Отражение
    signal = reflector[signal];
    
    // Проход назад через роторы (в обратном порядке)
    for (int i = rotors.size() - 1; i >= 0; i--) {
        signal = (signal + rotors[i].position) % 256;
        signal = rotors[i].inverseWiring[signal];
        signal = (signal - rotors[i].position + 256) % 256;
    }
    
    // Поворачиваем роторы после обработки байта
    rotateRotors();
    
    return static_cast<unsigned char>(signal);
}

void HughesCipher::initializeRotors(const std::string& key) {
    rotors.clear();
    std::istringstream iss(key);
    std::string token;
    std::vector<int> positions;
    
    while (std::getline(iss, token, ',')) {
        // Убираем пробелы
        token.erase(0, token.find_first_not_of(" \t"));
        token.erase(token.find_last_not_of(" \t") + 1);
        
        if (!token.empty()) {
            try {
                positions.push_back(std::stoi(token) % 256);
            } catch (...) {
                throw std::runtime_error("Invalid key format");
            }
        }
    }
    
    if (positions.size() != 3) {
        throw std::runtime_error("Hughes cipher requires 3 rotor positions");
    }
    
    // Создаем 3 ротора
    for (int i = 0; i < 3; i++) {
        Rotor rotor;
        
        // Инициализируем проводку
        rotor.wiring.resize(256);
        std::iota(rotor.wiring.begin(), rotor.wiring.end(), 0);
        
        // Перемешиваем проводку на основе seed
        std::shuffle(rotor.wiring.begin(), rotor.wiring.end(),
                    std::mt19937(positions[i]));
        
        // Создаем обратную проводку
        rotor.inverseWiring.resize(256);
        for (int j = 0; j < 256; j++) {
            rotor.inverseWiring[rotor.wiring[j]] = j;
        }
        
        rotor.position = positions[i];
        rotor.notch = (positions[i] + 67) % 256; // Произвольная защелка
        
        rotors.push_back(rotor);
    }
    
    initializeReflector();
}

void HughesCipher::initializeReflector() {
    reflector.resize(256);
    
    // Создаем парные отражения (каждый байт отражается в другой)
    for (int i = 0; i < 256; i++) {
        reflector[i] = 255 - i; // Простое отражение
    }
}

void HughesCipher::rotateRotors() {
    // Всегда поворачиваем первый ротор
    rotors[0].position = (rotors[0].position + 1) % 256;
    
    // Каскадный поворот для остальных роторов
    for (size_t i = 0; i < rotors.size() - 1; i++) {
        if (rotors[i].position == rotors[i].notch) {
            rotors[i + 1].position = (rotors[i + 1].position + 1) % 256;
        }
    }
}

std::string HughesCipher::generateKey() {
    return "1,2,54";
}

// C-interface for dynamic loading
extern "C" {
    void* createAlgorithm() {
        return new HughesCipher();
    }
    
    bool encrypt(void* algorithm, const char* inputFile, const char* outputFile, const char* key) {
        HughesCipher* cipher = static_cast<HughesCipher*>(algorithm);
        return cipher->encrypt(inputFile, outputFile, key);
    }
    
    bool decrypt(void* algorithm, const char* inputFile, const char* outputFile, const char* key) {
        HughesCipher* cipher = static_cast<HughesCipher*>(algorithm);
        return cipher->decrypt(inputFile, outputFile, key);
    }
    
    const char* encryptText(void* algorithm, const char* text, const char* key) {
        HughesCipher* cipher = static_cast<HughesCipher*>(algorithm);
        std::string result = cipher->encryptText(text, key);
        char* c_str = new char[result.length() + 1];
        std::strcpy(c_str, result.c_str());
        return c_str;
    }
    
    const char* decryptText(void* algorithm, const char* text, const char* key) {
        HughesCipher* cipher = static_cast<HughesCipher*>(algorithm);
        std::string result = cipher->decryptText(text, key);
        char* c_str = new char[result.length() + 1];
        std::strcpy(c_str, result.c_str());
        return c_str;
    }
    
    const char* generateKey(void* algorithm) {
        HughesCipher* cipher = static_cast<HughesCipher*>(algorithm);
        std::string result = cipher->generateKey();
        char* c_str = new char[result.length() + 1];
        std::strcpy(c_str, result.c_str());
        return c_str;
    }
    
    void freeString(const char* str) {
        delete[] str;
    }
    
    void destroyAlgorithm(void* algorithm) {
        delete static_cast<HughesCipher*>(algorithm);
    }
}

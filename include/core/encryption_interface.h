#pragma once
#include <string>
#include <vector>
//общий интерфейс для любых алгоритмов
class EncryptionInterface {
public:
    virtual ~EncryptionInterface() = default;
    virtual bool encrypt(const std::string& inputFile, const std::string& outputFile,
                        const std::string& key) = 0;
    virtual bool decrypt(const std::string& inputFile, const std::string& outputFile,
                        const std::string& key) = 0;
    virtual std::string encryptText(const std::string& text, const std::string& key) = 0;
    virtual std::string decryptText(const std::string& text, const std::string& key) = 0;
    virtual std::string generateKey() = 0;
    static std::vector<unsigned char> readFile(const std::string& filename);
    static bool writeFile(const std::string& filename, const std::vector<unsigned char>& data);
};

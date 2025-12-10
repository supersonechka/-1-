#pragma once
#include "core/encryption_interface.h"
#include <vector>
#include <string>

class HillCipher : public EncryptionInterface {
private:
    std::vector<int> parseKey(const std::string& key);
    int mod256(int value);
    void processData(std::vector<unsigned char>& data, const std::vector<int>& keyMatrix, bool isEncrypt);
    std::vector<int> getInverseMatrix(const std::vector<int>& keyMatrix);
    
public:
    bool encrypt(const std::string& inputFile, const std::string& outputFile,
                const std::string& key) override;
    bool decrypt(const std::string& inputFile, const std::string& outputFile,
                const std::string& key) override;
    
    std::string encryptText(const std::string& text, const std::string& key) override;
    std::string decryptText(const std::string& text, const std::string& key) override;
    
    std::string generateKey() override;
};

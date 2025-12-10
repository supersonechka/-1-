#pragma once
#include "core/encryption_interface.h"
#include <vector>
#include <string>

class PermutationCipher : public EncryptionInterface {
private:
    std::vector<size_t> parseKey(const std::string& key);
    std::vector<size_t> invertPermutation(const std::vector<size_t>& permutation);
    
public:
    bool encrypt(const std::string& inputFile, const std::string& outputFile,
                const std::string& key) override;
    bool decrypt(const std::string& inputFile, const std::string& outputFile,
                const std::string& key) override;
    
    std::string encryptText(const std::string& text, const std::string& key) override;
    std::string decryptText(const std::string& text, const std::string& key) override;
    
    std::string generateKey() override;
};

#pragma once
#include "core/encryption_interface.h"
#include <vector>
#include <string>

class HughesCipher : public EncryptionInterface {
private:
    struct Rotor {
        std::vector<int> wiring;
        std::vector<int> inverseWiring;
        int position;
        int notch;
    };
    
    std::vector<Rotor> rotors;
    std::vector<int> reflector;
    
    void initializeRotors(const std::string& key);
    void initializeReflector();
    unsigned char processByte(unsigned char input);
    void rotateRotors();
    
public:
    bool encrypt(const std::string& inputFile, const std::string& outputFile,
                const std::string& key) override;
    bool decrypt(const std::string& inputFile, const std::string& outputFile,
                const std::string& key) override;
    
    std::string encryptText(const std::string& text, const std::string& key) override;
    std::string decryptText(const std::string& text, const std::string& key) override;
    
    std::string generateKey() override;
};

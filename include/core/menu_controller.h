#pragma once
#include <string>

class MenuController {
public:
    void showMainMenu();
    void runAlgorithm(const std::string& algorithm);
    void processFile(const std::string& algorithm, const std::string& mode);
    void processText(const std::string& algorithm, const std::string& mode);
    
private:
    std::string selectFile();
    std::string getText();
    std::string getTextWithPrompt(const std::string& prompt);
    std::string getFixedKey(const std::string& algorithm);
    int getSafeIntInput();
};

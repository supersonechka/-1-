#pragma once
#include <string>
#include <vector>

class FileUtils {
public:
    static bool fileExists(const std::string& filename);
    static size_t getFileSize(const std::string& filename);
    static std::string getFileExtension(const std::string& filename);
    static std::string detectFileType(const std::vector<unsigned char>& data);
    static std::string getOutputFilename(const std::string& inputFile, const std::string& operation);
};

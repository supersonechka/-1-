#include "core/file_utils.h"
#include <fstream>
#include <filesystem>
#include <algorithm>

bool FileUtils::fileExists(const std::string& filename) {
    return std::filesystem::exists(filename);
}

size_t FileUtils::getFileSize(const std::string& filename) {
    return std::filesystem::file_size(filename);
}

std::string FileUtils::getFileExtension(const std::string& filename) {
    size_t dotPos = filename.find_last_of(".");
    if (dotPos != std::string::npos) {
        return filename.substr(dotPos + 1);
    }
    return "";
}

std::string FileUtils::detectFileType(const std::vector<unsigned char>& data) {
    if (data.size() < 8) return "bin";
    
    if (data[0] == 0xFF && data[1] == 0xD8 && data[2] == 0xFF)
        return "jpg";
    
    if (data[0] == 0x89 && data[1] == 0x50 && data[2] == 0x4E && data[3] == 0x47)
        return "png";
    
    if (data[0] == 0x25 && data[1] == 0x50 && data[2] == 0x44 && data[3] == 0x46)
        return "pdf";
    
    if (data[0] == 0x49 && data[1] == 0x44 && data[2] == 0x33)
        return "mp3";
    
    if (data[0] == 0x50 && data[1] == 0x4B && data[2] == 0x03 && data[3] == 0x04)
        return "zip";
    
    bool isText = true;
    for (size_t i = 0; i < std::min(size_t(100), data.size()); i++) {
        if (data[i] < 9 || (data[i] > 13 && data[i] < 32) && data[i] != 27) {
            isText = false;
            break;
        }
    }
    if (isText) return "txt";
    
    return "bin";
}

std::string FileUtils::getOutputFilename(const std::string& inputFile, const std::string& operation) {
    std::filesystem::path path(inputFile);
    std::string stem = path.stem().string();
    std::string extension = path.extension().string();
    
    if (operation == "encrypt") {
        return stem + "_encrypted" + extension;
    } else {
        if (stem.length() > 10 && stem.substr(stem.length() - 10) == "_encrypted") {
            return stem.substr(0, stem.length() - 10) + "_decrypted" + extension;
        } else {
            return stem + "_decrypted" + extension;
        }
    }
}

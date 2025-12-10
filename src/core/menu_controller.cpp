#include "core/menu_controller.h"
#include "core/file_utils.h"
#include "core/encryption_interface.h"
#include "core/binary_utils.h"
#include <iostream>
#include <memory>
#include <limits>
#include <cstring>
#include <locale>
#include <codecvt>

#ifdef _WIN32
    #include <windows.h>
    #define PLATFORM_WINDOWS
    #define LIB_SUFFIX ".dll"
    #define PATH_SEPARATOR "\\"
    #define LIB_PREFIX ""
#elif __APPLE__
    #include <dlfcn.h>
    #define PLATFORM_MACOS
    #define LIB_SUFFIX ".dylib"
    #define PATH_SEPARATOR "/"
    #define LIB_PREFIX "lib"
#elif __linux__
    #include <dlfcn.h>
    #define PLATFORM_LINUX
    #define LIB_SUFFIX ".so"
    #define PATH_SEPARATOR "/"
    #define LIB_PREFIX "lib"
#else
    #error "Unsupported platform"
#endif

typedef void* (*CreateAlgorithmFunc)();
typedef bool (*EncryptFunc)(void*, const char*, const char*, const char*);
typedef bool (*DecryptFunc)(void*, const char*, const char*, const char*);
typedef const char* (*EncryptTextFunc)(void*, const char*, const char*);
typedef const char* (*DecryptTextFunc)(void*, const char*, const char*);
typedef void (*DestroyFunc)(void*);
typedef void (*FreeStringFunc)(const char*);

namespace {
    std::string getLibraryPath(const std::string& algorithm) {
        #ifdef PLATFORM_WINDOWS
            return std::string(".\\") + algorithm + LIB_SUFFIX;
        #else
            return std::string("./lib/") + LIB_PREFIX + algorithm + LIB_SUFFIX;
        #endif
    }

    void setupConsole() {
        #ifdef PLATFORM_WINDOWS
            SetConsoleOutputCP(65001);
            SetConsoleCP(65001);
        #endif
    }

    std::string readUTF8String() {
        std::string text;
        
        #ifdef PLATFORM_WINDOWS
            std::wstring wtext;
            std::getline(std::wcin, wtext);
            
            std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
            try {
                text = converter.to_bytes(wtext);
            } catch (...) {
                text = "";
            }
        #else
            std::getline(std::cin, text);
        #endif
        
        return text;
    }
    
    void* loadLibrary(const std::string& path) {
        #ifdef PLATFORM_WINDOWS
            return LoadLibraryA(path.c_str());
        #else
            return dlopen(path.c_str(), RTLD_LAZY);
        #endif
    }

    void* getFunction(void* library, const char* name) {
        #ifdef PLATFORM_WINDOWS
            return GetProcAddress((HMODULE)library, name);
        #else
            return dlsym(library, name);
        #endif
    }
  
    void closeLibrary(void* library) {
        #ifdef PLATFORM_WINDOWS
            FreeLibrary((HMODULE)library);
        #else
            dlclose(library);
        #endif
    }

    std::string getLoadError() {
        #ifdef PLATFORM_WINDOWS
            DWORD error = GetLastError();
            return "Windows error code: " + std::to_string(error);
        #else
            const char* error = dlerror();
            return error ? error : "Unknown error";
        #endif
    }
}

int MenuController::getSafeIntInput() {
    int value;
    while (!(std::cin >> value)) {
        std::cout << "Ошибка! Введите число: ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return value;
}

std::string MenuController::getFixedKey(const std::string& algorithm) {
    if (algorithm == "hill") return "49,44,159,219";
    if (algorithm == "hughes") return "1,2,54";
    if (algorithm == "permutation") return "1,2,0";
    return "";
}

void MenuController::showMainMenu() {
    setupConsole();
    
    while (true) {
        std::cout << "=== Encryption Algoritms ===\n";
        std::cout << "1. Hill Cipher\n";
        std::cout << "2. Hughes Cipher\n";
        std::cout << "3. Permutation Cipher\n";
        std::cout << "4. Exit\n";
        std::cout << "Choose algorithm: ";
        
        int choice = getSafeIntInput();
        
        switch (choice) {
            case 1: runAlgorithm("hill"); break;
            case 2: runAlgorithm("hughes"); break;
            case 3: runAlgorithm("permutation"); break;
            case 4: return;
            default:
                std::cout << "Неверный выбор! Введите число от 1 до 4.\n";
                break;
        }
    }
}

void MenuController::runAlgorithm(const std::string& algorithm) {
    while (true) {
        std::cout << "\n=== " << algorithm << " Cipher ===\n";
        std::cout << "1. Encrypt File\n";
        std::cout << "2. Decrypt File\n";
        std::cout << "3. Encrypt Text\n";
        std::cout << "4. Decrypt Text\n";
        std::cout << "5. Back to main menu\n";
        std::cout << "Choose mode: ";
        
        int choice = getSafeIntInput();
        
        switch (choice) {
            case 1: processFile(algorithm, "encrypt"); break;
            case 2: processFile(algorithm, "decrypt"); break;
            case 3: processText(algorithm, "encrypt"); break;
            case 4: processText(algorithm, "decrypt"); break;
            case 5: return;
            default:
                std::cout << "Неверный выбор! Введите число от 1 до 5.\n";
                break;
        }
    }
}

void MenuController::processFile(const std::string& algorithm, const std::string& mode) {
    try {
        std::string inputFile = selectFile();
        if (inputFile.empty()) return;

        std::string outputFile = FileUtils::getOutputFilename(inputFile, mode);
        std::cout << "Выходной файл: " << outputFile << std::endl;

        std::string key = getFixedKey(algorithm);

        std::string libPath = getLibraryPath(algorithm);
        void* libraryHandle = loadLibrary(libPath.c_str());
        
        if (!libraryHandle) {
            std::cerr << "\nПредупреждение: Не удалось загрузить библиотеку: " << libPath << "\n";
            std::cerr << "Ошибка: " << getLoadError() << "\n";
            std::cerr << "Примечание: Если библиотека " << LIB_PREFIX << algorithm << LIB_SUFFIX;
            std::cerr << " удалена, шифрование не может быть выполнено по причине удаления плагина.\n\n";
            return;
        }

        CreateAlgorithmFunc createFunc = (CreateAlgorithmFunc)getFunction(libraryHandle, "createAlgorithm");
        EncryptFunc encryptFunc = (EncryptFunc)getFunction(libraryHandle, "encrypt");
        DecryptFunc decryptFunc = (DecryptFunc)getFunction(libraryHandle, "decrypt");
        DestroyFunc destroyFunc = (DestroyFunc)getFunction(libraryHandle, "destroyAlgorithm");

        if (!createFunc || !encryptFunc || !decryptFunc || !destroyFunc) {
            std::cerr << "Не удалось загрузить функции из библиотеки\n";
            closeLibrary(libraryHandle);
            return;
        }

        void* algorithmInstance = createFunc();
        bool success = false;
        
        if (mode == "encrypt") {
            success = encryptFunc(algorithmInstance, inputFile.c_str(), outputFile.c_str(), key.c_str());
            std::cout << (success ? "Шифрование успешно!\n" : "Шифрование не удалось!\n");
        } else {
            success = decryptFunc(algorithmInstance, inputFile.c_str(), outputFile.c_str(), key.c_str());
            std::cout << (success ? "Дешифрование успешно!\n" : "Дешифрование не удалось!\n");
        }

        destroyFunc(algorithmInstance);
        closeLibrary(libraryHandle);

    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
    }
}

void MenuController::processText(const std::string& algorithm, const std::string& mode) {
    try {
        std::string text;
        std::string numbers_input;
        
        if (mode == "encrypt") {
            std::cout << "Введите текст для шифрования: ";
            text = readUTF8String();
        } else {
            std::cout << "Введите текст для расшифрования: ";
            text = readUTF8String();
            numbers_input = text;
        }
        
        if (text.empty()) {
            std::cout << "Текст не может быть пустым!\n";
            return;
        }
        
        std::string key = getFixedKey(algorithm);

        std::string libPath = getLibraryPath(algorithm);
        void* libraryHandle = loadLibrary(libPath.c_str());
        
        if (!libraryHandle) {
            std::cerr << "\nПредупреждение: Не удалось загрузить библиотеку: " << libPath << "\n";
            std::cerr << "Ошибка: " << getLoadError() << "\n";
            std::cerr << "Примечание: Если библиотека " << LIB_PREFIX << algorithm << LIB_SUFFIX;
            std::cerr << " удалена, шифрование не может быть выполнено по причине удаления плагина.\n\n";
            return;
        }

        CreateAlgorithmFunc createFunc = (CreateAlgorithmFunc)getFunction(libraryHandle, "createAlgorithm");
        EncryptTextFunc encryptTextFunc = (EncryptTextFunc)getFunction(libraryHandle, "encryptText");
        DecryptTextFunc decryptTextFunc = (DecryptTextFunc)getFunction(libraryHandle, "decryptText");
        DestroyFunc destroyFunc = (DestroyFunc)getFunction(libraryHandle, "destroyAlgorithm");
        FreeStringFunc freeStringFunc = (FreeStringFunc)getFunction(libraryHandle, "freeString");
        
        if (!createFunc || !encryptTextFunc || !decryptTextFunc || !destroyFunc) {
            std::cerr << "Не удалось загрузить функции из библиотеки\n";
            closeLibrary(libraryHandle);
            return;
        }
        
        void* algorithmInstance = createFunc();
        const char* result = nullptr;
        
        if (mode == "encrypt") {
            result = encryptTextFunc(algorithmInstance, text.c_str(), key.c_str());
            std::string encrypted_str(result);
            
            std::cout << "\n=== Encription result ===\n";
            
            if (algorithm == "hill" || algorithm == "hughes") {
                std::vector<unsigned char> encrypted_data(encrypted_str.begin(), encrypted_str.end());
                std::string safe_result = BinaryUtils::binaryToSafeString(encrypted_data);
                std::cout << "Зашифрованные данные: " << safe_result << "\n";
            } else {
                std::cout << "Зашифрованный текст: " << encrypted_str << "\n";
            }
            
        } else {
            if (algorithm == "hill" || algorithm == "hughes") {
                std::vector<unsigned char> data_to_decrypt = BinaryUtils::safeStringToBinary(numbers_input);
                std::string text_to_decrypt(data_to_decrypt.begin(), data_to_decrypt.end());
                
                result = decryptTextFunc(algorithmInstance, text_to_decrypt.c_str(), key.c_str());
            } else {
                result = decryptTextFunc(algorithmInstance, text.c_str(), key.c_str());
            }
            
            std::cout << "\n=== Decription result ===\n";
            std::cout << "Decrypted text: " << result << "\n";
        }
        
        if (freeStringFunc && result) {
            freeStringFunc(result);
        }
        
        destroyFunc(algorithmInstance);
        closeLibrary(libraryHandle);
        
    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
    }
}

std::string MenuController::selectFile() {
    std::cout << "Введите имя входного файла: ";
    
    std::string filename = readUTF8String();
    
    if (!FileUtils::fileExists(filename)) {
        std::cout << "Файл не существует: " << filename << "\n";
        return "";
    }
    
    auto data = EncryptionInterface::readFile(filename);
    std::string fileType = FileUtils::detectFileType(data);
    
    std::cout << "Файл найден: " << filename << " ("
              << FileUtils::getFileSize(filename) << " байт, "
              << "тип: " << fileType << ")\n";
    return filename;
}

std::string MenuController::getText() {
    std::cout << "Введите текст: ";
    return readUTF8String();
}

std::string MenuController::getTextWithPrompt(const std::string& prompt) {
    std::cout << prompt;
    return readUTF8String();
}

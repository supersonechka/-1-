cmake_minimum_required(VERSION 3.10)
project(EncryptionRGR)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Базовые настройки
include_directories(include)
include_directories(src/core)

# Выходные директории
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/lib)

# Основная программа
add_executable(encryption_app
    src/core/main.cpp
    src/core/menu_controller.cpp
    src/core/file_utils.cpp
    src/core/encryption_interface.cpp
    src/core/binary_utils.cpp
)

# Общая библиотека утилит
add_library(encryption_utils STATIC
    src/core/file_utils.cpp
    src/core/encryption_interface.cpp
    src/core/binary_utils.cpp
)

# Алгоритмы как динамические библиотеки
add_library(hill SHARED src/algorithms/hill.cpp)
target_link_libraries(hill encryption_utils)

add_library(hughes SHARED src/algorithms/hughes.cpp)
target_link_libraries(hughes encryption_utils)

add_library(permutation SHARED src/algorithms/permutation.cpp)
target_link_libraries(permutation encryption_utils)

# Связываем основную программу
target_link_libraries(encryption_app encryption_utils)

# Кроссплатформенные настройки
if(WIN32)
    # Windows настройки
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
    target_link_libraries(encryption_app)
    set_target_properties(hill hughes permutation 
        PROPERTIES 
        SUFFIX ".dll"
    )
elseif(APPLE)
    # macOS настройки
    target_link_libraries(encryption_app dl)
    set_target_properties(hill hughes permutation 
        PROPERTIES 
        SUFFIX ".dylib"
        PREFIX "lib"
    )
elseif(UNIX)
    # Linux настройки
    target_link_libraries(encryption_app dl)
    set_target_properties(hill hughes permutation 
        PROPERTIES 
        SUFFIX ".so"
        PREFIX "lib"
    )
endif()

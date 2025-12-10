cmake_minimum_required(VERSION 3.16)

project(SimulateurParticules LANGUAGES CXX C)

# Standard C++ 17 requis pour Qt6
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_definitions(NOMINMAX)

find_package(Qt6 COMPONENTS Widgets REQUIRED)

include(FetchContent)
FetchContent_Declare(
    raylib
    DOWNLOAD_EXTRACT_TIMESTAMP OFF
    URL https://github.com/raysan5/raylib/archive/refs/tags/5.0.tar.gz
)
FetchContent_MakeAvailable(raylib)

# --- 3. Définition des sources ---
set(PROJECT_SOURCES
    src/main.cpp
    src/MainWindow.cpp
    src/MainWindow.h
)

# --- 4. Création de l'exécutable ---
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)


add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE 
    Qt6::Widgets 
    raylib
)

set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE ON)
cmake_minimum_required(VERSION 3.16)

# 1. On initialise le projet
project(SimulateurParticules LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- CONFIGURATION CUDA 13.0 ---

# 1. On garde le flag de compatibilité (au cas où VS2022 soit plus récent que CUDA 13)
set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -allow-unsupported-compiler")

# 2. On pointe VERS LA VERSION 13.0
set(CMAKE_CUDA_COMPILER "C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v13.0/bin/nvcc.exe")

# 3. Activation du langage
enable_language(CUDA)

# Architecture native (optimise pour votre RTX 3050)
set(CMAKE_CUDA_STANDARD 17)
set(CMAKE_CUDA_ARCHITECTURES "native")

add_compile_definitions(NOMINMAX)

find_package(Qt6 COMPONENTS Widgets REQUIRED)
find_package(CUDAToolkit REQUIRED)

include(FetchContent)
FetchContent_Declare(
    raylib
    DOWNLOAD_EXTRACT_TIMESTAMP OFF
    URL https://github.com/raysan5/raylib/archive/refs/tags/5.0.tar.gz
)
FetchContent_MakeAvailable(raylib)

set(PROJECT_SOURCES
    src/main.cpp
    src/MainWindow.cpp
    src/MainWindow.h
    src/RaylibWidget.cpp
    src/RaylibWidget.h
    src/ParticleKernel.h
    src/ParticleKernel.cu
)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} "src/Particle.h")

target_link_libraries(${PROJECT_NAME} PRIVATE 
    Qt6::Widgets 
    raylib
    CUDA::cudart
)

# Laissez commenté pour voir les erreurs dans la console noire
# set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE ON)
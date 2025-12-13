cmake_minimum_required(VERSION 3.16)

# On définit le projet
project(SimulateurParticules LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- CONFIGURATION CUDA 13.0 ---
# Force la compatibilité avec VS 2022
set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -allow-unsupported-compiler")
# Force le chemin du compilateur (ajustez si besoin)
set(CMAKE_CUDA_COMPILER "C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v13.0/bin/nvcc.exe")

enable_language(CUDA)

set(CMAKE_CUDA_STANDARD 17)
set(CMAKE_CUDA_ARCHITECTURES "native")

add_compile_definitions(NOMINMAX)

# --- DEPENDANCES ---
find_package(Qt6 COMPONENTS Widgets REQUIRED)
find_package(CUDAToolkit REQUIRED)

# --- RAYLIB (Via GIT) ---
include(FetchContent)
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 5.0
)
# Cette commande rend Raylib disponible pour le projet
FetchContent_MakeAvailable(raylib)

# --- SOURCES ---
set(PROJECT_SOURCES
    src/main.cpp
    src/MainWindow.cpp
    src/MainWindow.h
    src/RaylibWidget.cpp
    src/RaylibWidget.h
    src/ParticleKernel.h
    src/ParticleKernel.cu
    src/Particle.h
    CMakeLists.txt
)

# Configuration automatique Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# --- EXECUTABLE ---
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

# --- LIENS ---
target_link_libraries(${PROJECT_NAME} PRIVATE 
    Qt6::Widgets 
    raylib
    CUDA::cudart
)
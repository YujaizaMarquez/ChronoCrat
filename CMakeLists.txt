cmake_minimum_required(VERSION 3.24)

project(MiProyectoSFML LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Coloca el ejecutable final siempre en build/bin/ (tanto en Windows como en Linux)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Compilar SFML de forma estática evita problemas de archivos .dll faltantes en Windows
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_AUDIO ON CACHE BOOL "" FORCE)
set(SFML_BUILD_NETWORK OFF CACHE BOOL "" FORCE)

include(FetchContent)

FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.1.0 # Puedes ajustarlo a 3.0.0 o la rama 3.x según disponibilidad del tag exacto
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(SFML)

# Busca automáticamente todos los .cpp en la carpeta src/
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp")

add_executable(${PROJECT_NAME} ${SOURCES})

# Incluye la carpeta include/ para tus propios archivos .hpp / .h
target_include_directories(${PROJECT_NAME} PRIVATE include)

target_link_libraries(${PROJECT_NAME} PRIVATE
    SFML::Graphics
    SFML::Audio
)
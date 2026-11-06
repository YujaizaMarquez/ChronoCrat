cmake_minimum_required(VERSION 3.24)

project(MiProyectoSFML LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Coloca el ejecutable final siempre en build/bin/ (tanto en Windows como en Linux)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Compilar SFML y el runtime de MinGW de forma estática evita DLL faltantes o incompatibles en Windows
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_AUDIO OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_NETWORK OFF CACHE BOOL "" FORCE)

include(FetchContent)

FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.1.0 # Puedes ajustarlo a 3.0.0 o la rama 3.x según disponibilidad del tag exacto
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(SFML)

set(DESIGNAR_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)
set(DESIGNAR_BUILD_TESTS OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    Designar
    GIT_REPOSITORY https://github.com/R3mmurd/DeSiGNAR.git
    GIT_TAG v4.0.1
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(Designar)

# Busca automáticamente todos los .cpp en la carpeta src/
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp")

add_executable(${PROJECT_NAME} ${SOURCES})

# Incluye la carpeta include/ para tus propios archivos .hpp / .h
target_include_directories(${PROJECT_NAME} PRIVATE include)

target_link_libraries(${PROJECT_NAME} PRIVATE
    SFML::Graphics
    Designar::Designar
)

if(MINGW)
    target_link_options(${PROJECT_NAME} PRIVATE -static)
endif()

file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/assets"
    DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
)
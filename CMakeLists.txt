cmake_minimum_required(VERSION 3.10)

# Указываем путь к Vcpkg
project(LuckyBilliard)
set(CMAKE_TOOLCHAIN_FILE "C:/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")

# Настройки компилятора
set(CMAKE_CXX_STANDARD 17)

# Ищем библиотеки
find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

# Создаём исполняемый файл
add_executable(game src/main.cpp)

# Подключаем библиотеки
target_link_libraries(game PRIVATE
    glfw
    glad::glad
    glm::glm
    opengl32.lib
)

# Для Windows добавляем OpenGL
if(WIN32)
    target_link_libraries(game PRIVATE opengl32.lib)
endif()
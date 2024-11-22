cmake_minimum_required(VERSION 3.10)

project(DataCollector)

# Указываем путь к инструментам для vcpkg (если используете vcpkg)
set(CMAKE_TOOLCHAIN_FILE /path/to/vcpkg/scripts/buildsystems/vcpkg.cmake)

# Находим и подключаем библиотеку nlohmann-json
find_package(nlohmann_json CONFIG REQUIRED)

# Указываем путь к заголовочным файлам Modbus
include_directories(/usr/include /usr/include/modbus)

# Указываем путь к библиотекам Modbus
link_directories(/usr/lib/x86_64-linux-gnu)

# Указываем исходные файлы для сборки
add_executable(DataCollector DataCollector.cpp)

# Подключаем библиотеки
target_link_libraries(DataCollector PRIVATE nlohmann_json::nlohmann_json modbus)

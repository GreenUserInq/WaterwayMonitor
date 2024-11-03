# Минимальная версия CMake
cmake_minimum_required(VERSION 3.8)

# Название проекта
project("DataCollector")

# Установка пути к toolchain-файлу vcpkg
if (NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE "C:/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif()

# Добавление исполняемого файла
add_executable(DataCollector "DataCollector.cpp" "DataCollector.h")

# Установка стандарта C++
if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET DataCollector PROPERTY CXX_STANDARD 20)
endif()

# Подключение путей к заголовочным файлам и библиотекам для modbus
target_include_directories(DataCollector PRIVATE "C:/vcpkg/installed/x64-windows/include")
target_link_libraries(DataCollector PRIVATE "C:/vcpkg/installed/x64-windows/lib/modbus.lib")

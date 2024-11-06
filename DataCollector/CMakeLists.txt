cmake_minimum_required(VERSION 3.10)
project(DataCollector)

# Подключите библиотеку libmodbus с использованием pkg-config
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBMODBUS REQUIRED libmodbus)

# Укажите пути к заголовочным файлам и библиотекам libmodbus
include_directories(${LIBMODBUS_INCLUDE_DIRS})
link_directories(${LIBMODBUS_LIBRARY_DIRS})

# Укажите исходные файлы проекта
add_executable(DataCollector DataCollector.cpp)

# Слинкуйте библиотеку libmodbus с вашим приложением
target_link_libraries(DataCollector ${LIBMODBUS_LIBRARIES})

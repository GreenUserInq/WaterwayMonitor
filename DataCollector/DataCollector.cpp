#include <nlohmann/json.hpp>
#include <modbus.h>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>

// Используем alias для удобства
using json = nlohmann::json;

// Функция для записи данных в файл JSON
void writeMonitoringData(const std::string& file_path, uint16_t minLevel, uint16_t deviation) {
    json root;
    std::ifstream file_in(file_path);
    if (file_in.is_open()) {
        file_in >> root;
        file_in.close();
    }

    // Обновление данных
    root["WaterLevel"] = minLevel;
    root["HumidityInsideThePipe"] = deviation;

    // Запись обновленных данных в файл
    std::ofstream file_out(file_path);
    if (file_out.is_open()) {
        file_out << root.dump(4);  // Форматированный вывод с отступами
        file_out.close();
        std::cout << "Данные записаны в файл: " << file_path << "\n";
    }
    else {
        std::cerr << "Не удалось открыть файл для записи: " << file_path << "\n";
    }
}

// Функция для чтения из регистра Modbus
bool readRegister(modbus_t* ctx, int slaveID, int regAddress, uint16_t& value) {
    if (modbus_set_slave(ctx, slaveID) == -1) {
        std::cerr << "Ошибка установки slave ID " << slaveID << ": " << modbus_strerror(errno) << "\n";
        return false;
    }

    if (modbus_read_registers(ctx, regAddress, 1, &value) == -1) {
        std::cerr << "Ошибка чтения регистра " << regAddress << " у устройства " << slaveID << ": "
            << modbus_strerror(errno) << "\n";
        return false;
    }
    return true;
}

// Функция для опроса первого датчика уровня
bool pollLevelSensor1(modbus_t* ctx, int slaveID, uint16_t& level) {
    return readRegister(ctx, slaveID, 0x0100, level);  // Чтение регистра для уровня
}

// Функция для опроса второго датчика уровня
bool pollLevelSensor2(modbus_t* ctx, int slaveID, uint16_t& level) {
    return readRegister(ctx, slaveID, 0x0100, level);  // Чтение регистра для уровня
}

// Основная функция
int main() {
    std::string file_path = "../../../../MonitoringData.json";

    // Настройки для Modbus
    modbus_t* ctx1 = modbus_new_rtu("/dev/ttyS0", 9600, 'N', 8, 1); // Настройте порт
    modbus_t* ctx2 = modbus_new_rtu("/dev/ttyS1", 9600, 'N', 8, 1); // Настройте порт

    // Подключение к Modbus
    if (modbus_connect(ctx1) == -1 || modbus_connect(ctx2) == -1) {
        std::cerr << "Ошибка подключения к Modbus" << std::endl;
        return -1;
    }

    // Чтение данных с датчиков уровня
    uint16_t level1 = 0, level2 = 0;
    if (pollLevelSensor1(ctx1, 1, level1) && pollLevelSensor2(ctx2, 2, level2)) {
        // Вычисление минимального уровня и отклонения
        uint16_t minLevel = std::min(level1, level2);
        uint16_t maxLevel = std::max(level1, level2);
        uint16_t deviation = maxLevel - minLevel;

        // Запись данных в JSON файл
        writeMonitoringData(file_path, minLevel, deviation);
    }
    else {
        std::cerr << "Ошибка опроса датчиков уровня" << std::endl;
        return -1;
    }

    // Закрытие соединений
    modbus_close(ctx1);
    modbus_close(ctx2);
    modbus_free(ctx1);
    modbus_free(ctx2);

    return 0;
}


//save_to_file(data, "../../../../MonitoringData.json");
//для отображения
    // Вывод загруженных данных
    /*std::cout << "ID: " << loaded_data.ID << "\n"
        << "WaterLevel: " << loaded_data.WaterLevel << "\n"
        << "DegreeOfClogging: " << loaded_data.DegreeOfClogging << "\n"
        << "StructuralDeformations: " << loaded_data.StructuralDeformations << "\n"
        << "AmbientTemperature: " << loaded_data.AmbientTemperature << "\n"
        << "WaterFlowRate: " << loaded_data.WaterFlowRate << "\n"
        << "HumidityInsideThePipe: " << loaded_data.HumidityInsideThePipe << std::endl;*/
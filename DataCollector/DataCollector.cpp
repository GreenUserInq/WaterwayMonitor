#include <nlohmann/json.hpp>
#include <modbus.h>
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <thread>
#include <sys/stat.h> // Для проверки изменения файла конфигурации

using json = nlohmann::json;

// Прототип функции для проверки изменения файла
bool isConfigUpdated(const std::string& config_path, time_t& last_mod_time);

// Структура для хранения параметров датчика
struct SensorConfig {
    int slaveID;
    std::string port;
    int baudRate;
    char parity;
    int dataBits;
    int stopBits;
    std::string registerAddress;
};

// Функция для чтения конфигурации
int readConfig(const std::string& config_path, std::vector<SensorConfig>& sensors) {
    std::ifstream config_file(config_path);
    if (!config_file.is_open()) {
        std::cerr << "Не удалось открыть файл конфигурации: " << config_path << "\n";
        return 5000; // Значение по умолчанию для интервала
    }

    json config_json;
    config_file >> config_json;
    config_file.close();

    if (config_json.contains("IntervalMs") && config_json["IntervalMs"].is_number()) {
        int interval = config_json["IntervalMs"].get<int>();

        // Загружаем конфигурацию для датчиков
        if (config_json.contains("Sensors") && config_json["Sensors"].is_array()) {
            for (const auto& sensor_json : config_json["Sensors"]) {
                SensorConfig sensor;
                sensor.slaveID = sensor_json["SlaveID"].get<int>();
                sensor.port = sensor_json["Port"].get<std::string>();
                sensor.baudRate = sensor_json["BaudRate"].get<int>();
                sensor.parity = sensor_json["Parity"].get<std::string>()[0]; // Для символьного типа
                sensor.dataBits = sensor_json["DataBits"].get<int>();
                sensor.stopBits = sensor_json["StopBits"].get<int>();
                sensor.registerAddress = sensor_json["RegisterAddress"].get<std::string>();
                sensors.push_back(sensor);
            }
        }

        return interval;
    }
    else {
        std::cerr << "Некорректный файл конфигурации. Используется значение по умолчанию.\n";
        return 5000;
    }
}

// Функция для проверки изменения файла
bool isConfigUpdated(const std::string& config_path, time_t& last_mod_time) {
    struct stat file_stat;
    if (stat(config_path.c_str(), &file_stat) != 0) {
        std::cerr << "Не удалось получить информацию о файле конфигурации: " << config_path << "\n";
        return false;
    }

    if (file_stat.st_mtime != last_mod_time) {
        last_mod_time = file_stat.st_mtime;
        return true;
    }
    return false;
}

// Функция для записи данных в файл JSON
void writeMonitoringData(const std::string& file_path, uint16_t deviation) {
    json root;
    std::ifstream file_in(file_path);
    if (file_in.is_open()) {
        file_in >> root;
        file_in.close();
    }

    root["WaterLevel"] = deviation;

    std::ofstream file_out(file_path);
    if (file_out.is_open()) {
        file_out << root.dump(4);
        file_out.close();
        std::cout << "Данные записаны в файл: " << file_path << "\n";
    }
    else {
        std::cerr << "Не удалось открыть файл для записи: " << file_path << "\n";
    }
}

// Функция для чтения из регистра Modbus
bool readRegister(modbus_t* ctx, const SensorConfig& sensor, uint16_t& value) {
    if (modbus_set_slave(ctx, sensor.slaveID) == -1) {
        std::cerr << "Ошибка установки slave ID " << sensor.slaveID << ": " << modbus_strerror(errno) << "\n";
        return false;
    }

    int regAddress = std::stoi(sensor.registerAddress, nullptr, 16); // Преобразуем адрес из строки в число
    if (modbus_read_registers(ctx, regAddress, 1, &value) == -1) {
        std::cerr << "Ошибка чтения регистра " << regAddress << " у устройства " << sensor.slaveID << ": "
            << modbus_strerror(errno) << "\n";
        return false;
    }
    return true;
}

// Основная функция
int main() {
    std::string file_path = "../MonitoringData.json";
    std::string config_path = "../config.json";

    time_t last_mod_time = 0; // Время последней модификации конфигурационного файла
    std::vector<SensorConfig> sensors;
    int interval_ms = readConfig(config_path, sensors); // Чтение интервала и конфигурации датчиков

    std::vector<modbus_t*> contexts;
    for (const auto& sensor : sensors) {
        modbus_t* ctx = modbus_new_rtu(sensor.port.c_str(), sensor.baudRate, sensor.parity, sensor.dataBits, sensor.stopBits);
        if (!ctx) {
            std::cerr << "Ошибка создания контекста Modbus для датчика " << sensor.slaveID << "\n";
            return -1;
        }
        contexts.push_back(ctx);
    }

    // Подключение к Modbus
    for (auto& ctx : contexts) {
        if (modbus_connect(ctx) == -1) {
            std::cerr << "Ошибка подключения к Modbus\n";
            return -1;
        }
    }

    while (true) {
        // Проверяем изменения в конфигурационном файле
        if (isConfigUpdated(config_path, last_mod_time)) {
            interval_ms = readConfig(config_path, sensors);
            std::cout << "Интервал обновлён: " << interval_ms << " мс\n";
        }

        // Опрос датчиков
        uint16_t level1 = 0, level2 = 0;
        if (readRegister(contexts[0], sensors[0], level1) && readRegister(contexts[1], sensors[1], level2)) {
            uint16_t deviation = std::abs(static_cast<int>(level1) - static_cast<int>(level2));

            writeMonitoringData(file_path, deviation);
        }
        else {
            std::cerr << "Ошибка опроса датчиков\n";
        }

        // Пауза на указанный интервал
        std::this_thread::sleep_for(std::chrono::milliseconds(interval_ms));
    }

    // Закрытие соединений
    for (auto& ctx : contexts) {
        modbus_close(ctx);
        modbus_free(ctx);
    }

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
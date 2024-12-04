#include <nlohmann/json.hpp>
#include <modbus.h>
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <thread>
#include <sys/stat.h>
#include <vector>
#include <algorithm> // Для std::min и std::abs

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
    std::string commandRegister; // Регистр для отправки команды
    uint16_t commandValue;       // Значение команды
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
            sensors.clear();
            for (const auto& sensor_json : config_json["Sensors"]) {
                SensorConfig sensor;
                sensor.slaveID = sensor_json["SlaveID"].get<int>();
                sensor.port = sensor_json["Port"].get<std::string>();
                sensor.baudRate = sensor_json["BaudRate"].get<int>();
                sensor.parity = sensor_json["Parity"].get<std::string>()[0];
                sensor.dataBits = sensor_json["DataBits"].get<int>();
                sensor.stopBits = sensor_json["StopBits"].get<int>();
                sensor.registerAddress = sensor_json["RegisterAddress"].get<std::string>();
                if (sensor_json.contains("CommandRegister")) {
                    sensor.commandRegister = sensor_json["CommandRegister"].get<std::string>();
                    sensor.commandValue = sensor_json["CommandValue"].get<uint16_t>();
                }
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

// Функция для отправки команды на датчик
bool sendCommand(modbus_t* ctx, const SensorConfig& sensor) {
    if (modbus_set_slave(ctx, sensor.slaveID) == -1) {
        std::cerr << "Ошибка установки slave ID " << sensor.slaveID << ": " << modbus_strerror(errno) << "\n";
        return false;
    }

    int commandRegister = std::stoi(sensor.commandRegister, nullptr, 16);
    if (modbus_write_register(ctx, commandRegister, sensor.commandValue) == -1) {
        std::cerr << "Ошибка отправки команды в регистр " << commandRegister << " у устройства " << sensor.slaveID
            << ": " << modbus_strerror(errno) << "\n";
        return false;
    }

    // Небольшая пауза для завершения измерения
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    return true;
}

// Функция для записи данных в файл JSON
void writeMonitoringData(const std::string& file_path,
    uint16_t waterLevel1, uint16_t waterLevel2,
    uint16_t clogging1, uint16_t clogging2,
    uint16_t header1, uint16_t header2,
    uint16_t deformations,
    const std::string& alarmState,
    const std::string& errors) {
    json root;
    std::ifstream file_in(file_path);
    if (file_in.is_open()) {
        file_in >> root;
        file_in.close();
    }

    root["WaterLevel1"] = waterLevel1;
    root["WaterLevel2"] = waterLevel2;
    root["DegreeOfClogging1"] = clogging1;
    root["DegreeOfClogging2"] = clogging2;
    root["CloggingHeader1"] = header1;
    root["CloggingHeader2"] = header2;
    root["StructuralDeformations"] = deformations;
    root["AlarmState"] = alarmState;
    root["Errors"] = errors;

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

    int regAddress = std::stoi(sensor.registerAddress, nullptr, 16);
    if (modbus_read_registers(ctx, regAddress, 1, &value) == -1) {
        std::cerr << "Ошибка чтения регистра " << regAddress << " у устройства " << sensor.slaveID << ": "
            << modbus_strerror(errno) << "\n";
        return false;
    }
    return true;
}

// Основная функция
int main() {
    std::string file_path = "/home/avads/WaterwayMonitoring/MonitoringData.json";
    std::string config_path = "/home/avads/WaterwayMonitoring/config.json";

    time_t last_mod_time = 0;
    std::vector<SensorConfig> sensors;
    int interval_ms = readConfig(config_path, sensors);

    std::vector<modbus_t*> contexts;
    for (const auto& sensor : sensors) {
        modbus_t* ctx = modbus_new_rtu(sensor.port.c_str(), sensor.baudRate, sensor.parity, sensor.dataBits, sensor.stopBits);
        if (!ctx) {
            std::cerr << "Ошибка создания контекста Modbus для датчика " << sensor.slaveID << "\n";
            continue;
        }
        if (modbus_connect(ctx) == -1) {
            std::cerr << "Ошибка подключения к Modbus для датчика " << sensor.slaveID << "\n";
            modbus_free(ctx);
            continue;
        }
        contexts.push_back(ctx);
    }

    while (true) {
        if (isConfigUpdated(config_path, last_mod_time)) {
            interval_ms = readConfig(config_path, sensors);
            std::cout << "Интервал обновлён: " << interval_ms << " мс\n";
        }

        uint16_t waterLevel1 = 0, waterLevel2 = 0;
        uint16_t clogging1 = 0, clogging2 = 0;
        uint16_t header1 = 0, header2 = 0;
        uint16_t deformations = 0;
        std::string alarmState = "No alarms";
        std::string errors = "";

        // Считывание данных с первого датчика уровня
        if (!readRegister(contexts[0], sensors[0], waterLevel1)) {
            errors += "WaterLevel1 - нет связи; ";
            waterLevel1 = 0;
        }

        // Считывание данных со второго датчика уровня
        if (!readRegister(contexts[1], sensors[1], waterLevel2)) {
            errors += "WaterLevel2 - нет связи; ";
            waterLevel2 = 0;
        }

        // Считывание данных с первого датчика засоренности
        if (!readRegister(contexts[2], sensors[2], clogging1)) {
            errors += "DegreeOfClogging1 - нет связи; ";
            clogging1 = 0;
        }

        // Считывание данных со второго датчика засоренности
        if (!readRegister(contexts[3], sensors[3], clogging2)) {
            errors += "DegreeOfClogging2 - нет связи; ";
            clogging2 = 0;
        }

        // Считывание данных с первого датчика засоренности оголовков
        if (!sendCommand(contexts[4], sensors[4]) || !readRegister(contexts[4], sensors[4], header1)) {
            errors += "CloggingHeader1 - нет связи; ";
            header1 = 0;
        }

        // Считывание данных со второго датчика засоренности оголовков
        if (!sendCommand(contexts[5], sensors[5]) || !readRegister(contexts[5], sensors[5], header2)) {
            errors += "CloggingHeader2 - нет связи; ";
            header2 = 0;
        }

        // Считывание данных с датчика деформации
        if (!readRegister(contexts[6], sensors[6], deformations)) {
            errors += "StructuralDeformations - нет связи; ";
            deformations = 0;
        }

        // Если ошибок нет, записываем "No errors detected"
        if (errors.empty()) {
            errors = "No errors detected";
        }

        // Запись данных в файл
        writeMonitoringData(file_path,
            waterLevel1, waterLevel2,
            clogging1, clogging2,
            header1, header2,
            deformations,
            alarmState,
            errors);

        // Задержка перед следующим циклом
        std::this_thread::sleep_for(std::chrono::milliseconds(interval_ms));
    }


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
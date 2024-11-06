#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <modbus.h>
#include <vector> 
#include <cerrno>


/// <summary>
/// вспомогательная функция, которая читает данные из заданного диапазона регистров. 
/// Она принимает контекст Modbus, начальный адрес, количество регистров и массив для хранения данных.
/// </summary>
/// <param name="ctx"></param>
/// <param name="startAddress"></param>
/// <param name="numRegisters"></param>
/// <param name="data"></param>
/// <returns></returns>
bool readSensorData(modbus_t* ctx, int startAddress, int numRegisters, std::vector<uint16_t>& data) {
    data.resize(numRegisters);
    if (modbus_read_registers(ctx, startAddress, numRegisters, data.data()) == -1) {
        std::cerr << "Ошибка чтения данных с датчика: " << modbus_strerror(errno) << "\n";
        return false;
    }
    return true;
}

/// <summary>
/// Функция для чтения данных с одного датчика по его адресу на линии
/// </summary>
/// <param name="ctx"></param>
/// <param name="slaveID"></param>
/// <param name="startAddress"></param>
/// <param name="numRegisters"></param>
/// <param name="data"></param>
/// <returns></returns>
bool readSensorData1(modbus_t* ctx, int slaveID, int startAddress, int numRegisters, std::vector<uint16_t>& data) {
    data.resize(numRegisters);
    // Устанавливаем адрес устройства (slave ID)
    if (modbus_set_slave(ctx, slaveID) == -1) {
        std::cerr << "Ошибка установки slave ID " << slaveID << ": " << modbus_strerror(errno) << "\n";
        return false;
    }

    // Чтение данных с устройства
    if (modbus_read_registers(ctx, startAddress, numRegisters, data.data()) == -1) {
        std::cerr << "Ошибка чтения данных с устройства " << slaveID << ": " << modbus_strerror(errno) << "\n";
        return false;
    }
    return true;
}

struct Data {
    int ID;
    int WaterLevel;
    int DegreeOfClogging;
    int StructuralDeformations;
    int AmbientTemperature;
    int WaterFlowRate;
    int HumidityInsideThePipe;

    // Сериализация структуры Data в JSON
    std::string to_json() const {
        std::ostringstream oss;
        oss << "{\n";
        oss << "  \"ID\": " << ID << ",\n";
        oss << "  \"WaterLevel\": " << WaterLevel << ",\n";
        oss << "  \"DegreeOfClogging\": " << DegreeOfClogging << ",\n";
        oss << "  \"StructuralDeformations\": " << StructuralDeformations << ",\n";
        oss << "  \"AmbientTemperature\": " << AmbientTemperature << ",\n";
        oss << "  \"WaterFlowRate\": " << WaterFlowRate << ",\n";
        oss << "  \"HumidityInsideThePipe\": " << HumidityInsideThePipe << "\n";
        oss << "}";
        return oss.str();
    }

    // Десериализация из JSON строки в структуру Data
    static Data from_json(const std::string& json_str) {
        Data data;
        std::istringstream iss(json_str);
        std::string line;

        while (std::getline(iss, line)) {
            if (line.find("\"ID\"") != std::string::npos)
                data.ID = std::stoi(line.substr(line.find(":") + 1));
            else if (line.find("\"WaterLevel\"") != std::string::npos)
                data.WaterLevel = std::stoi(line.substr(line.find(":") + 1));
            else if (line.find("\"DegreeOfClogging\"") != std::string::npos)
                data.DegreeOfClogging = std::stoi(line.substr(line.find(":") + 1));
            else if (line.find("\"StructuralDeformations\"") != std::string::npos)
                data.StructuralDeformations = std::stoi(line.substr(line.find(":") + 1));
            else if (line.find("\"AmbientTemperature\"") != std::string::npos)
                data.AmbientTemperature = std::stoi(line.substr(line.find(":") + 1));
            else if (line.find("\"WaterFlowRate\"") != std::string::npos)
                data.WaterFlowRate = std::stoi(line.substr(line.find(":") + 1));
            else if (line.find("\"HumidityInsideThePipe\"") != std::string::npos)
                data.HumidityInsideThePipe = std::stoi(line.substr(line.find(":") + 1));
        }

        return data;
    }
};

// Пример записи в файл
void save_to_file(const Data& data, const std::string& file_path) {
    std::ofstream file(file_path);
    if (file.is_open()) {
        file << data.to_json();
        file.close();
    }
}

// Пример чтения из файла
Data load_from_file(const std::string& file_path) {
    std::ifstream file(file_path);
    std::string json_str;
    if (file.is_open()) {
        std::getline(file, json_str, '\0');  // Чтение всего файла в строку
        file.close();
    }
    return Data::from_json(json_str);
}

int main() {
    // Пример данных
    Data data = { 0, 0, 0, 0, 0, 0, 0 };
    Data loaded_data = load_from_file("../../../../MonitoringData.json");
    data = { 1, 0, 0, 0, 0, 0, 0 };

//#pragma region
//    // Параметры подключения для каждого датчика (IP остается одним и тем же, но порт разный)
//    const char* ip_address = "127.0.0.1";  // IP-адрес устройства Modbus
//    std::vector<int> ports = { 502, 503, 504, 505, 506, 507 }; // Порты для каждого датчика
//
//    // Адреса начальных регистров для каждого датчика
//    std::vector<int> sensorAddresses = { 0, 10, 20, 30, 40, 50 };
//    std::vector<uint16_t> sensorData;
//
//    // Перебор каждого порта для подключения к отдельному датчику
//    for (size_t i = 0; i < ports.size(); ++i) {
//        // Создаем контекст Modbus для подключения к конкретному порту
//        modbus_t* ctx = modbus_new_tcp(ip_address, ports[i]);
//        if (ctx == nullptr) {
//            std::cerr << "Не удалось создать контекст Modbus для порта " << ports[i] << "\n";
//            continue;
//        }
//
//        // Подключаемся к устройству
//        if (modbus_connect(ctx) == -1) {
//            std::cerr << "Ошибка подключения к порту " << ports[i] << ": " << modbus_strerror(errno) << "\n";
//            modbus_free(ctx);
//            continue;
//        }
//
//        std::cout << "Чтение данных с датчика, подключенного к порту " << ports[i] << "...\n";
//
//        // Чтение данных с датчика
//        if (readSensorData(ctx, sensorAddresses[i], 2, sensorData)) {
//            std::cout << "Данные с датчика на порту " << ports[i] << ": ";
//            for (auto value : sensorData) {
//                std::cout << value << " ";
//            }
//            std::cout << "\n";
//        }
//
//        // Завершение работы с данным портом
//        modbus_close(ctx);
//        modbus_free(ctx);
//
//
//    }
//#pragma endregion
#pragma region
    modbus_t* ctx = modbus_new_tcp("127.0.0.1", 502);
    if (ctx == nullptr) {
        std::cerr << "Не удалось создать контекст Modbus\n";
        return -1;
    }

    // Подключение к Modbus-серверу
    if (modbus_connect(ctx) == -1) {
        std::cerr << "Ошибка подключения: " << modbus_strerror(errno) << "\n";
        modbus_free(ctx);
        return -1;
    }

    // Параметры для двух датчиков
    std::vector<int> slaveIDs = { 1, 2 };        // ID для каждого датчика на линии
    int startAddress = 0;                      // Начальный адрес данных
    int numRegisters = 2;                      // Количество регистров для чтения

    for (int slaveID : slaveIDs) {
        std::vector<uint16_t> sensorData;
        if (readSensorData1(ctx, slaveID, startAddress, numRegisters, sensorData)) {
            std::cout << "Данные с датчика ID " << slaveID << ": ";
            for (auto value : sensorData) {
                std::cout << value << " ";
            }
            std::cout << "\n";
        }
    }
#pragma endregion

    save_to_file(data, "../../../../MonitoringData.json");
    return 0;
}

//для отображения
    // Вывод загруженных данных
    /*std::cout << "ID: " << loaded_data.ID << "\n"
        << "WaterLevel: " << loaded_data.WaterLevel << "\n"
        << "DegreeOfClogging: " << loaded_data.DegreeOfClogging << "\n"
        << "StructuralDeformations: " << loaded_data.StructuralDeformations << "\n"
        << "AmbientTemperature: " << loaded_data.AmbientTemperature << "\n"
        << "WaterFlowRate: " << loaded_data.WaterFlowRate << "\n"
        << "HumidityInsideThePipe: " << loaded_data.HumidityInsideThePipe << std::endl;*/
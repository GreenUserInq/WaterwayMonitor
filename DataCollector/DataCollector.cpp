#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

// Указываем полный путь к modbus.h



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
    Data data = { 1, 13, 0, 4, 71, 26, 1 };

    save_to_file(data, "../../../../MonitoringData.json");
    Data loaded_data = load_from_file("../../../../MonitoringData.json");


    // Вывод загруженных данных
    std::cout << "ID: " << loaded_data.ID << "\n"
        << "WaterLevel: " << loaded_data.WaterLevel << "\n"
        << "DegreeOfClogging: " << loaded_data.DegreeOfClogging << "\n"
        << "StructuralDeformations: " << loaded_data.StructuralDeformations << "\n"
        << "AmbientTemperature: " << loaded_data.AmbientTemperature << "\n"
        << "WaterFlowRate: " << loaded_data.WaterFlowRate << "\n"
        << "HumidityInsideThePipe: " << loaded_data.HumidityInsideThePipe << std::endl;

    return 0;
}

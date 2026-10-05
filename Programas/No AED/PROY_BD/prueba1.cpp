#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip>
#include <regex>

struct Column {
    std::string name;
    std::string type;
    bool isCalculated = false;
    int intPart = 0;           // Número de dígitos enteros (para DECIMAL)
    int decimalPart = 0;       // Número de dígitos decimales (para DECIMAL)
};

int main() {
    // Abrir el archivo struct_table.txt
    std::ifstream file("struct_table.txt");
    if (!file.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo 'struct_table.txt'." << std::endl;
        return 1;
    }

    std::string line;
    std::vector<Column> columns;
    std::regex decimalRegex(R"(DECIMAL\((\d+),\s*(\d+)\))"); // Regex para DECIMAL(n, m)
    std::regex genericRegex(R"((\w+)\((\d+)\))");            // Regex genérico para tipos como INTEGER(n) o VARCHAR(n)

    while (std::getline(file, line)) {
        if (line.find("CREATE TABLE") != std::string::npos) {
            continue; // Saltar declaración de la tabla
        }

        if (line.find(");") != std::string::npos) {
            break; // Detenerse al final de la tabla
        }

        std::stringstream ss(line);
        std::string name, type;
        ss >> name >> type;

        Column column = {name, type};

        // Procesar tipo DECIMAL(n, m)
        std::smatch match;
        if (std::regex_search(line, match, decimalRegex)) {
            column.type = "DECIMAL";
            column.intPart = std::stoi(match[1]);
            column.decimalPart = std::stoi(match[2]);
        }
        // Procesar otros tipos con parámetros (como INTEGER(10) o VARCHAR(40))
        else if (std::regex_search(line, match, genericRegex)) {
            column.type = match[1]; // Extraer el nombre del tipo
        }

        columns.push_back(column);
    }

    file.close();

    // Mostrar estructura de la tabla
    std::cout << "Estructura de la tabla:" << std::endl;
    for (const auto& column : columns) {
        std::cout << "Columna: " << column.name << ", Tipo: " << column.type;
        if (column.type == "DECIMAL") {
            std::cout << " (" << column.intPart << ", " << column.decimalPart << ")";
        }
        std::cout << std::endl;
    }

    return 0;
}

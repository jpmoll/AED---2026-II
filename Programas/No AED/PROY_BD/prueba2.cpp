#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip> // Para std::fixed y std::setprecision
#include <cmath>   // Para std::round

struct Column {
    std::string name;
    std::string type;
    int size; // Tamaño en bytes del tipo
};

// Función para calcular el tamaño de un tipo de dato
int calculateSize(const std::string& type) {
    if (type.find("INTEGER") != std::string::npos) {
        return 4; // Un entero ocupa 4 bytes
    }
    if (type.find("DECIMAL") != std::string::npos) {
        return 8; // Un número decimal (double) ocupa 8 bytes
    }
    if (type.find("VARCHAR") != std::string::npos) {
        size_t start = type.find('(');
        size_t end = type.find(')');
        if (start != std::string::npos && end != std::string::npos) {
            return std::stoi(type.substr(start + 1, end - start - 1));
        }
    }
    return 0; // Tipo desconocido
}

// Función para obtener el siguiente índice automáticamente
int getNextIndex(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return 1; // Archivo no existe o está vacío, comenzar desde 1
    }

    std::string line, lastLine;
    while (std::getline(file, line)) {
        lastLine = line;
    }
    file.close();

    if (lastLine.empty()) {
        return 1; // Archivo vacío, comenzar desde 1
    }

    // Extraer el índice desde la última línea
    std::stringstream ss(lastLine);
    std::string indexStr;
    std::getline(ss, indexStr, ','); // Leer el índice (primer campo)
    return std::stoi(indexStr) + 1;
}

// Función para leer las filas del archivo CSV
std::vector<std::vector<std::pair<std::string, int>>> readCSV(const std::vector<Column>& columns, const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo '" << filename << "'." << std::endl;
        return {};
    }

    std::vector<std::vector<std::pair<std::string, int>>> rows;
    std::string line;

    // Leer cada fila del archivo
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string value;
        std::vector<std::pair<std::string, int>> row;

        // Leer los valores de la fila y calcular tamaños
        for (const auto& column : columns) {
            if (!std::getline(ss, value, ',')) {
                value = ""; // Si no hay más datos, completar con vacío
            }

            int size = column.size; // Tamaño predefinido según el tipo
            if (column.type.find("VARCHAR") != std::string::npos) {
                size = value.size(); // Calcular tamaño real para VARCHAR
            }

            row.emplace_back(value, size);
        }

        rows.push_back(row);
    }

    file.close();
    return rows;
}

int main() {
    // Leer la estructura de la tabla desde struct_table.txt
    std::ifstream structFile("struct_table.txt");
    if (!structFile.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo 'struct_table.txt'." << std::endl;
        return 1;
    }

    std::string line;
    std::vector<Column> columns;

    while (std::getline(structFile, line)) {
        if (line.find("CREATE TABLE") != std::string::npos) {
            continue; // Saltar declaración de la tabla
        }

        if (line.find(");") != std::string::npos) {
            break; // Detenerse al final de la tabla
        }

        std::stringstream ss(line);
        std::string name, type;
        ss >> name >> type;

        Column column = {name, type, calculateSize(type)};

        columns.push_back(column);
    }

    structFile.close();

    // Menú interactivo
    int option;
    do {
        std::cout << "\nMenu:\n1. Mostrar filas\n0. Salir\nSeleccione una opción: ";
        std::cin >> option;
        if (option == 1) {
            auto rows = readCSV(columns, "taxables.csv");
            std::cout << "\nDatos del archivo 'taxables.csv':\n";
            for (const auto& row : rows) {
                std::cout << "[ ";
                for (const auto& [value, size] : row) {
                    std::cout << "(" << value << ", " << size << " bytes) ";
                }
                std::cout << "]\n";
            }
        } else if (option != 0) {
            std::cout << "Opción no válida." << std::endl;
        }
    } while (option != 0);

    return 0;
}

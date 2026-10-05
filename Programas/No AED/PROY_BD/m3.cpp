#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <iomanip>
#include <algorithm>
#include <cctype>

// *** Clases relacionadas al Disco ***

class Sector {
public:
    int capacidad;
    int ocupado;
    std::vector<std::string> datos;

    Sector(int capacidad = 64) : capacidad(capacidad), ocupado(0) {}

    // Almacenar fragmentos de un dato y devolver cuántos bytes pudieron almacenarse
    int almacenarFragmento(const std::string& dato, int offset) {
        int espacioDisponible = capacidad - ocupado;
        int bytesAGuardar = std::min(espacioDisponible, (int)(dato.size() - offset));
        datos.push_back(dato.substr(offset, bytesAGuardar));
        ocupado += bytesAGuardar;
        return bytesAGuardar;
    }

    void mostrarEstado() const {
        std::cout << "(" << ocupado << " - " << capacidad << ")";
    }
};

class Pista {
public:
    std::vector<Sector> sectores;

    Pista(int numSectores, int capacidadSector) {
        for (int i = 0; i < numSectores; ++i) {
            sectores.emplace_back(capacidadSector);
        }
    }
};

class Superficie {
public:
    std::vector<Pista> pistas;

    Superficie(int numPistas, int numSectores, int capacidadSector) {
        for (int i = 0; i < numPistas; ++i) {
            pistas.emplace_back(numSectores, capacidadSector);
        }
    }
};

class Disco {
public:
    int numDiscos;
    int numSuperficies;
    std::vector<std::vector<Superficie>> discos; // [Disco][Superficie]
    std::unordered_map<std::string, std::vector<std::tuple<int, int, int, int>>> indice; // Map para búsqueda (nombre -> posiciones)

    Disco(int numDiscos, int numPistas, int numSectores, int capacidadSector) : numDiscos(numDiscos), numSuperficies(2) {
        for (int d = 0; d < numDiscos; ++d) {
            std::vector<Superficie> superficies;
            for (int s = 0; s < numSuperficies; ++s) {
                superficies.emplace_back(numPistas, numSectores, capacidadSector);
            }
            discos.push_back(superficies);
        }
    }

    void almacenarDatos(const std::vector<std::pair<std::string, int>>& datos) {
        int discoIdx = 0, superficieIdx = 0, pistaIdx = 0, sectorIdx = 0;

        for (const auto& [dato, size] : datos) {
            int offset = 0;
            while (offset < size) {
                auto& sector = discos[discoIdx][superficieIdx].pistas[pistaIdx].sectores[sectorIdx];
                int almacenado = sector.almacenarFragmento(dato, offset);
                indice[dato].emplace_back(discoIdx + 1, pistaIdx + 1, sectorIdx + 1, almacenado);
                offset += almacenado;

                // Mover al siguiente sector si el actual está lleno
                if (sector.ocupado == sector.capacidad) {
                    sectorIdx++;
                    if (sectorIdx >= discos[discoIdx][superficieIdx].pistas[pistaIdx].sectores.size()) {
                        sectorIdx = 0;
                        pistaIdx++;
                        if (pistaIdx >= discos[discoIdx][superficieIdx].pistas.size()) {
                            pistaIdx = 0;
                            superficieIdx++;
                            if (superficieIdx >= numSuperficies) {
                                superficieIdx = 0;
                                discoIdx++;
                                if (discoIdx >= numDiscos) {
                                    std::cerr << "Error: No hay mas espacio en los discos.\n";
                                    return;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    void mostrarEstructura() const {
        for (size_t d = 0; d < discos.size(); ++d) {
            std::cout << "Disco " << d + 1 << ":\n";
            for (size_t s = 0; s < discos[d].size(); ++s) {
                std::cout << "  Superficie " << s + 1 << ":\n";
                for (size_t p = 0; p < discos[d][s].pistas.size(); ++p) {
                    std::cout << "    Pista " << p + 1 << ":\n";
                    for (size_t k = 0; k < discos[d][s].pistas[p].sectores.size(); ++k) {
                        std::cout << "      Sector " << k + 1 << " ";
                        discos[d][s].pistas[p].sectores[k].mostrarEstado();
                        std::cout << "\n";
                    }
                }
            }
        }
    }

    // Convertir una cadena a minúsculas
    static std::string toLower(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) {
            return std::tolower(c);
        });
        return result;
    }

    void buscarDatoPorNombre(const std::string& nombre) const {
        std::string nombreLower = toLower(nombre);
        std::cout << "Resultados de la busqueda para '" << nombre << "':\n";
        bool encontrado = false;

        for (const auto& [dato, posiciones] : indice) {
            if (toLower(dato).rfind(nombreLower, 0) == 0) { // Verificar si comienza con 'nombreLower'
                encontrado = true;
                std::cout << "Dato: " << dato << "\n";
                for (const auto& [disco, pista, sector, size] : posiciones) {
                    std::cout << "  Disco: " << disco << ", Pista: " << pista << ", Sector: " << sector << ", Tamano: " << size << " bytes\n";
                }
            }
        }

        if (!encontrado) {
            std::cout << "No se encontraron coincidencias para '" << nombre << "'.\n";
        }
    }

    // buscar en cualquier posicion de la cadena
    /*void buscarDatoPorNombre(const std::string& nombre) const {       
        std::string nombreLower = toLower(nombre);
        std::cout << "Resultados de la busqueda para '" << nombre << "':\n";
        bool encontrado = false;

        for (const auto& [dato, posiciones] : indice) {
            if (toLower(dato).find(nombreLower) != std::string::npos) {
                encontrado = true;
                std::cout << "Dato: " << dato << "\n";
                for (const auto& [disco, pista, sector, size] : posiciones) {
                    std::cout << "  Disco: " << disco << ", Pista: " << pista << ", Sector: " << sector << ", Tamano: " << size << " bytes\n";
                }
            }
        }

        if (!encontrado) {
            std::cout << "No se encontraron coincidencias para '" << nombre << "'.\n";
        }
    }*/
};

// *** Funciones para manejar el archivo CSV ***

std::vector<std::pair<std::string, int>> extraerDatosCSV(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo '" << filename << "'.\n";
        return {};
    }

    std::vector<std::pair<std::string, int>> datos;
    std::string line;
    bool isHeader = true;

    while (std::getline(file, line)) {
        if (isHeader) {
            isHeader = false; // Omitir la primera fila (encabezados)
            continue;
        }

        std::stringstream ss(line);
        std::string value;
        while (std::getline(ss, value, ',')) {
            datos.emplace_back(value, value.size());
        }
    }

    file.close();
    return datos;
}

// *** Programa principal con menú ***

int main() {
    int numDiscos = 1, numPistas = 5, numSectores = 10, capacidadSector = 64;
    Disco disco(numDiscos, numPistas, numSectores, capacidadSector);
    std::vector<std::pair<std::string, int>> datos = extraerDatosCSV("taxables.csv");
    disco.almacenarDatos(datos);

    int option;

    do {
        std::cout << "\nMenu:\n1. Modificar configuracion del disco\n2. Mostrar estado del disco\n3. Buscar datos por nombre\n0. Salir\nSeleccione una opcion: ";
        std::cin >> option;

        if (option == 1) {
            std::cout << "Ingrese el número de discos: ";
            std::cin >> numDiscos;
            std::cout << "Ingrese el número de pistas: ";
            std::cin >> numPistas;
            std::cout << "Ingrese el número de sectores por pista: ";
            std::cin >> numSectores;
            std::cout << "Ingrese la capacidad de cada sector (bytes): ";
            std::cin >> capacidadSector;

            disco = Disco(numDiscos, numPistas, numSectores, capacidadSector);
            disco.almacenarDatos(datos);
            std::cout << "Configuracion actualizada y datos recargados.\n";
        } else if (option == 2) {
            disco.mostrarEstructura();
        } else if (option == 3) {
            std::string nombre;
            std::cout << "Ingrese el nombre o parte del dato a buscar: ";
            std::cin.ignore();
            std::getline(std::cin, nombre);
            disco.buscarDatoPorNombre(nombre);
        } else if (option != 0) {
            std::cout << "Opcion no valida.\n";
        }
    } while (option != 0);

    return 0;
}

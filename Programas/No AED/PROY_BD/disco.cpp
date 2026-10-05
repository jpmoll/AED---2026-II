#include <iostream>
#include <vector>
#include <string>
#include <iomanip> // Para formato de salida

class Sector {
private:
    int capacidad;     // Capacidad máxima en bytes
    int ocupado;       // Espacio ocupado en bytes

public:
    Sector(int capacidad = 64) : capacidad(capacidad), ocupado(0) {}

    // Modificar capacidad del sector
    void setCapacidad(int nuevaCapacidad) {
        capacidad = nuevaCapacidad;
        if (ocupado > capacidad) {
            ocupado = capacidad; // Ajustar espacio ocupado si excede la nueva capacidad
        }
    }

    // Agregar datos al sector
    void almacenarDatos(int bytes) {
        if (ocupado + bytes <= capacidad) {
            ocupado += bytes;
        } else {
            std::cout << "No hay suficiente espacio en el sector (espacio disponible: " << (capacidad - ocupado) << " bytes).\n";
        }
    }

    // Mostrar espacio ocupado
    void mostrarEstado() const {
        std::cout << "(" << ocupado << " - " << capacidad << ")";
    }
};

class Pista {
public:
    std::vector<Sector> sectores;

    Pista(int numSectores, int capacidadSector) {
        for (int i = 0; i < numSectores; ++i) {
            sectores.emplace_back(capacidadSector); // Crear sectores con la capacidad especificada
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
private:
    int numSuperficies; // Siempre 2
    std::vector<Superficie> superficies;

public:
    Disco(int numPistas = 5, int numSectores = 10, int capacidadSector = 64) : numSuperficies(2) {
        for (int i = 0; i < numSuperficies; ++i) {
            superficies.emplace_back(numPistas, numSectores, capacidadSector);
        }
    }

    // Mostrar la estructura del disco
    void mostrarEstructura() const {
        for (int i = 0; i < numSuperficies; ++i) {
            std::cout << "Superficie " << i + 1 << ":" << std::endl;
            for (size_t j = 0; j < superficies[i].pistas.size(); ++j) {
                std::cout << "  Pista " << j + 1 << ":" << std::endl;
                for (size_t k = 0; k < superficies[i].pistas[j].sectores.size(); ++k) {
                    std::cout << "    Sector " << k + 1 << " ";
                    superficies[i].pistas[j].sectores[k].mostrarEstado();
                    std::cout << std::endl;
                }
            }
        }
    }
};

int main() {
    int numDiscos, numPistas, numSectores, capacidadSector;

    std::cout << "Ingrese el número de discos: ";
    std::cin >> numDiscos;

    std::cout << "Ingrese el número de pistas por superficie (por defecto 5): ";
    std::cin >> numPistas;

    std::cout << "Ingrese el número de sectores por pista (por defecto 10): ";
    std::cin >> numSectores;

    std::cout << "Ingrese la capacidad de cada sector en bytes (por defecto 64): ";
    std::cin >> capacidadSector;

    // Crear discos con la configuración del usuario
    std::vector<Disco> discos;
    for (int i = 0; i < numDiscos; ++i) {
        discos.emplace_back(numPistas, numSectores, capacidadSector);
    }

    // Mostrar estructura de los discos
    for (int i = 0; i < numDiscos; ++i) {
        std::cout << "\nDisco " << i + 1 << ":" << std::endl;
        discos[i].mostrarEstructura();
    }

    return 0;
}

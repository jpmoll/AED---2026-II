#include <iostream>
#include <vector>
#include <tuple>
#include <string>
using namespace std;

// Clase Sector
struct Sector {
    bool ocupado;
    int sizeUsed;       // Tamaño ocupado en el sector
    int fragmentID;     // Identificador del fragmento
    int fragmentOrder;  // Orden del fragmento dentro del dato
    Sector() : ocupado(false), sizeUsed(0), fragmentID(-1), fragmentOrder(-1) {}
};

// Clase Disco
class Disk {
    int numPlatos, numSuperficies, numPistas, numSectores, sectorSize;
    vector<vector<vector<vector<Sector>>>> storage;

public:
    // Constructor del Disco
    Disk(int platos, int superficies, int pistas, int sectores, int tamSector)
        : numPlatos(platos), numSuperficies(superficies), 
          numPistas(pistas), numSectores(sectores), sectorSize(tamSector) {
        // Inicialización de la estructura del disco
        storage.resize(platos, vector<vector<vector<Sector>>>(
                                superficies, vector<vector<Sector>>(
                                                pistas, vector<Sector>(sectores))));
    }

    // Obtener el tamaño de un sector
    int getSectorSize() const {
        return sectorSize;
    }

    // Guardar datos fragmentados
    bool saveData(int dataID, const string& data) {
        int dataSize = data.size();
        int sectoresNecesarios = (dataSize + sectorSize - 1) / sectorSize; // Redondeo hacia arriba
        int currentFragment = 0;

        // Buscar sectores libres
        for (int p = 0; p < numPlatos; ++p) {
            for (int s = 0; s < numSuperficies; ++s) {
                for (int t = 0; t < numPistas; ++t) {
                    for (int sec = 0; sec < numSectores; ++sec) {
                        if (!storage[p][s][t][sec].ocupado && currentFragment < sectoresNecesarios) {
                            // Calcular el tamaño del fragmento actual
                            int fragmentSize = min(sectorSize, dataSize - (currentFragment * sectorSize));

                            // Ocupar el sector
                            storage[p][s][t][sec].ocupado = true;
                            storage[p][s][t][sec].sizeUsed = fragmentSize;
                            storage[p][s][t][sec].fragmentID = dataID;
                            storage[p][s][t][sec].fragmentOrder = currentFragment;

                            ++currentFragment;
                        }
                    }
                }
            }
        }

        if (currentFragment < sectoresNecesarios) {
            cout << "Error: No hay suficiente espacio en el disco para almacenar los datos.\n";
            return false;
        }

        cout << "Datos guardados correctamente con ID " << dataID << ".\n";
        return true;
    }

    // Leer datos por ID
    string readData(int dataID) {
        string reconstructedData;

        for (int p = 0; p < numPlatos; ++p) {
            for (int s = 0; s < numSuperficies; ++s) {
                for (int t = 0; t < numPistas; ++t) {
                    for (int sec = 0; sec < numSectores; ++sec) {
                        if (storage[p][s][t][sec].fragmentID == dataID) {
                            int fragmentSize = storage[p][s][t][sec].sizeUsed;

                            // Simular lectura del fragmento (aquí solo generamos caracteres 'x' por simplicidad)
                            reconstructedData += string(fragmentSize, 'x');
                        }
                    }
                }
            }
        }

        if (reconstructedData.empty()) {
            cout << "Error: Datos con ID " << dataID << " no encontrados.\n";
        } else {
            cout << "Datos reconstruidos correctamente con ID " << dataID << ".\n";
        }

        return reconstructedData;
    }

    // Mostrar el estado del disco
    void displayDiskState() const {
        for (int p = 0; p < numPlatos; ++p) {
            for (int s = 0; s < numSuperficies; ++s) {
                for (int t = 0; t < numPistas; ++t) {
                    for (int sec = 0; sec < numSectores; ++sec) {
                        const Sector& sector = storage[p][s][t][sec];
                        cout << "Plato: " << p 
                             << ", Superficie: " << s
                             << ", Pista: " << t
                             << ", Sector: " << sec
                             << " -> Ocupado: " << (sector.ocupado ? "Sí" : "No")
                             << ", Tamaño Usado: " << sector.sizeUsed << " bytes"
                             << ", Fragment ID: " << sector.fragmentID
                             << ", Fragment Order: " << sector.fragmentOrder << "\n";
                    }
                }
            }
        }
    }
};

int main() {
    // Crear un disco con 2 platos, 2 superficies, 2 pistas, 3 sectores y cada sector de 50 bytes
    Disk disk(2, 2, 2, 3, 50);

    // Mostrar estado inicial del disco
    cout << "Estado inicial del disco:\n";
    disk.displayDiskState();

    // Guardar datos en el disco
    cout << "\nGuardando datos en el disco...\n";
    disk.saveData(1, string(120, 'A')); // Datos de 120 bytes
    disk.saveData(2, string(80, 'B')); // Datos de 80 bytes

    // Mostrar estado del disco después de guardar
    cout << "\nEstado del disco después de guardar datos:\n";
    disk.displayDiskState();

    // Leer datos del disco
    cout << "\nLeyendo datos del disco...\n";
    string data1 = disk.readData(1);
    string data2 = disk.readData(2);

    cout << "Datos leídos para ID 1: " << data1 << "\n";
    cout << "Datos leídos para ID 2: " << data2 << "\n";

    return 0;
}

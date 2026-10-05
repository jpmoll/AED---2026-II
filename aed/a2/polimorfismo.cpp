#include <iostream>
#include <vector>
#include <cmath> // Para usar M_PI

/*Añade una nueva figura geométrica, como un triángulo, y asegúrate de que implemente correctamente el método calcularArea().

Modifica el programa para que también imprima los parámetros de cada figura (radio en el caso del círculo, ancho y alto en el caso del rectángulo).*/

// Clase abstracta Figura
class Figura {
public:
    virtual double calcularArea() const = 0; // Método virtual puro
    virtual void mostrar() const = 0;
    virtual ~Figura() {} // Destructor virtual para limpiar correctamente
};

// Clase derivada Circulo
class Circulo : public Figura {
private:
    double radio;
public:
    Circulo(double r) : radio(r) {}
    
    double calcularArea() const override {
        return 3.1415 * radio * radio;
    }

    void mostrar() const override{
        std::cout << radio << std::endl;
    }
};

// Clase derivada Rectangulo
class Rectangulo : public Figura {
private:
    double ancho;
    double alto;
public:
    Rectangulo(double a, double b) : ancho(a), alto(b) {}

    double calcularArea() const override {
        return ancho * alto;
    }
    void mostrar() const override{
        std::cout << ancho << std::endl;
        std::cout << alto << std::endl;
    }
};

class triangulo : public Figura{
    private:
        double base;
        double altura;
    public:
        triangulo(double b, double h){
            base = b;
            altura=h;
        }
        double calcularArea() const override{
            return (base*altura)/2;
        }
        void mostrar() const override{
            std::cout << base << std::endl;
            std::cout << altura << std::endl;
        }
};

// Función principal
int main() {
    // Crear un vector de punteros a Figura
    std::vector<Figura*> figuras;

    // Agregar un círculo y un rectángulo
    figuras.push_back(new Circulo(5.0));      // Radio 5.0
    figuras.push_back(new Rectangulo(4.0, 3.0));
    figuras.push_back(new triangulo(4.0, 3.0));  // Ancho 4.0, Alto 3.0

    // Calcular y mostrar el área de cada figura
    for (size_t i = 0; i < figuras.size(); i++) {
        std::cout << "Área de la figura " << i + 1 << ": " 
                  << figuras[i]->calcularArea() << std::endl;
        std::cout << "Parametros: " << std::endl;
        figuras[i]->mostrar();
    }

    // Liberar la memoria asignada dinámicamente
    for (size_t i = 0; i < figuras.size(); i++) {
        delete figuras[i];
    }

    return 0;
}

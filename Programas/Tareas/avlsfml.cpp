#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>

using namespace std;

class Cnode {
public:
    int value;
    int height;
    Cnode* a[2]; // [0]: left, [1]: right

    Cnode(int v) {
        value = v;
        height = 1;
        a[0] = a[1] = nullptr;
    }
};

class Arbol {
public:
    Cnode* Root = nullptr;

    // Función para obtener la altura de un nodo
    int getHeight(Cnode* node) {
        return node ? node->height : 0;
    }

    // Función para calcular el factor de balance
    int getBalance(Cnode* node) {
        return node ? getHeight(node->a[0]) - getHeight(node->a[1]) : 0;
    }

    // Rotación derecha
    Cnode* rotateRight(Cnode* y) {
        Cnode* x = y->a[0];
        Cnode* T = x->a[1];

        x->a[1] = y;
        y->a[0] = T;

        y->height = max(getHeight(y->a[0]), getHeight(y->a[1])) + 1;
        x->height = max(getHeight(x->a[0]), getHeight(x->a[1])) + 1;

        return x;
    }

    // Rotación izquierda
    Cnode* rotateLeft(Cnode* x) {
        Cnode* y = x->a[1];
        Cnode* T = y->a[0];

        y->a[0] = x;
        x->a[1] = T;

        x->height = max(getHeight(x->a[0]), getHeight(x->a[1])) + 1;
        y->height = max(getHeight(y->a[0]), getHeight(y->a[1])) + 1;

        return y;
    }

    // Inserción balanceada
    Cnode* insert(Cnode* node, int v) {
        if (!node)
            return new Cnode(v);

        if (v < node->value)
            node->a[0] = insert(node->a[0], v);
        else if (v > node->value)
            node->a[1] = insert(node->a[1], v);
        else
            return node;

        node->height = 1 + max(getHeight(node->a[0]), getHeight(node->a[1]));

        int balance = getBalance(node);

        // Rotaciones necesarias
        if (balance > 1 && v < node->a[0]->value)
            return rotateRight(node);

        if (balance < -1 && v > node->a[1]->value)
            return rotateLeft(node);

        if (balance > 1 && v > node->a[0]->value) {
            node->a[0] = rotateLeft(node->a[0]);
            return rotateRight(node);
        }

        if (balance < -1 && v < node->a[1]->value) {
            node->a[1] = rotateRight(node->a[1]);
            return rotateLeft(node);
        }

        return node;
    }

    void Agregar(int v) {
        Root = insert(Root, v);
    }

    void drawTree(sf::RenderWindow& window, Cnode* node, int x, int y, int xOffset, int level) {
        if (!node)
            return;

        int nodeRadius = 30;

        // Dibuja las líneas entre nodos
        if (node->a[0] != nullptr) {
            sf::Vertex line[] = {
                sf::Vertex(sf::Vector2f(x, y)),
                sf::Vertex(sf::Vector2f(x - xOffset, y + 100))
            };
            window.draw(line, 2, sf::Lines);
            drawTree(window, node->a[0], x - xOffset, y + 100, xOffset / 2, level + 1);
        }
        if (node->a[1] != nullptr) {
            sf::Vertex line[] = {
                sf::Vertex(sf::Vector2f(x, y)),
                sf::Vertex(sf::Vector2f(x + xOffset, y + 100))
            };
            window.draw(line, 2, sf::Lines);
            drawTree(window, node->a[1], x + xOffset, y + 100, xOffset / 2, level + 1);
        }

        // Dibuja el nodo actual
        sf::CircleShape circle(nodeRadius);
        circle.setFillColor(sf::Color::Red);
        circle.setPosition(x - nodeRadius, y - nodeRadius);
        window.draw(circle);

        // Muestra el valor del nodo
        sf::Font font;
        if (!font.loadFromFile("C:/WINDOWS/FONTS/arial.ttf")) {
            cerr << "Error loading font!" << endl;
            return;
        }

        sf::Text text;
        text.setFont(font);
        text.setString(to_string(node->value));
        text.setCharacterSize(25);
        text.setFillColor(sf::Color::Black);
        text.setPosition(x - 10, y - 10);
        window.draw(text);
    }
};

int main() {
    Arbol tree;

    tree.Agregar(10);
    tree.Agregar(20);
    tree.Agregar(30);
    tree.Agregar(40);
    tree.Agregar(50);
    tree.Agregar(25);
    tree.Agregar(11);
    tree.Agregar(0);
    tree.Agregar(2);
    tree.Agregar(19);
    tree.Agregar(32);
    tree.Agregar(77);
    tree.Agregar(13);
    tree.Agregar(69);
    tree.Agregar(90);
    tree.Agregar(5);
    tree.Agregar(7);
    tree.Agregar(14);

    sf::RenderWindow window(sf::VideoMode(1200, 600), "AVL Tree Visualization");

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        tree.drawTree(window, tree.Root, 600, 50, 200, 0);
        window.display();
    }

    return 0;
}

#include <SFML/Graphics.hpp>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>
#include <string>
#include <vector>

using namespace std;

struct Cnode {
    int value;
    int height;
    Cnode* a[2]; // [0]: left, [1]: right
    Cnode(int v) : value(v), height(1) { a[0] = a[1] = nullptr; }
};

struct Familia {
    Cnode* padre = nullptr;
    Cnode* abuelo = nullptr;
    Cnode* tio = nullptr;
    vector<Cnode*> primos;
};

class Arbol {
    Cnode* Root = nullptr;

    int getHeight(Cnode* n) { return n ? n->height : 0; }
    int getBalance(Cnode* n) { return n ? getHeight(n->a[0]) - getHeight(n->a[1]) : 0; }

    Cnode* rotateRight(Cnode* y) {
        Cnode* x = y->a[0];
        Cnode* T = x->a[1];
        x->a[1] = y;
        y->a[0] = T;
        y->height = max(getHeight(y->a[0]), getHeight(y->a[1])) + 1;
        x->height = max(getHeight(x->a[0]), getHeight(x->a[1])) + 1;
        return x;
    }

    Cnode* rotateLeft(Cnode* x) {
        Cnode* y = x->a[1];
        Cnode* T = y->a[0];
        y->a[0] = x;
        x->a[1] = T;
        x->height = max(getHeight(x->a[0]), getHeight(x->a[1])) + 1;
        y->height = max(getHeight(y->a[0]), getHeight(y->a[1])) + 1;
        return y;
    }

    Cnode* insert(Cnode* node, int v) {
        if (!node) return new Cnode(v);
        if (v < node->value)      node->a[0] = insert(node->a[0], v);
        else if (v > node->value) node->a[1] = insert(node->a[1], v);
        else return node;

        node->height = 1 + max(getHeight(node->a[0]), getHeight(node->a[1]));
        int balance = getBalance(node);

        if (balance > 1 && v < node->a[0]->value)  return rotateRight(node);
        if (balance < -1 && v > node->a[1]->value) return rotateLeft(node);
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

    void destroy(Cnode* n) {
        if (!n) return;
        destroy(n->a[0]);
        destroy(n->a[1]);
        delete n;
    }

    // x = índice in-order, y = profundidad -> nunca se solapan
    void layoutRec(Cnode* n, int depth, int& counter,
                   map<Cnode*, sf::Vector2f>& pos) const {
        if (!n) return;
        layoutRec(n->a[0], depth + 1, counter, pos);
        pos[n] = { 50.f + counter++ * 70.f, 90.f + depth * 90.f };
        layoutRec(n->a[1], depth + 1, counter, pos);
    }

public:
    Arbol() = default;
    Arbol(const Arbol&) = delete;
    Arbol& operator=(const Arbol&) = delete;
    ~Arbol() { destroy(Root); }

    void Agregar(int v) { Root = insert(Root, v); }
    Cnode* root() const { return Root; }

    void layout(map<Cnode*, sf::Vector2f>& pos) const {
        pos.clear();
        int counter = 0;
        layoutRec(Root, 0, counter, pos);
    }

    // Padre, abuelo, tío y primos del nodo con ese valor.
    // Primos = hijos del tío (el otro hijo del abuelo).
    // Devuelve false si el valor no está en el árbol.
    bool familia(int value, Familia& f) const {
        vector<Cnode*> camino;
        Cnode* n = Root;
        while (n) {
            camino.push_back(n);
            if (value == n->value) break;
            n = value < n->value ? n->a[0] : n->a[1];
        }
        if (!n) return false;

        f = Familia();
        size_t k = camino.size();
        if (k >= 2) f.padre = camino[k - 2];
        if (k >= 3) {
            f.abuelo = camino[k - 3];
            f.tio = (f.abuelo->a[0] == f.padre) ? f.abuelo->a[1] : f.abuelo->a[0];
            if (f.tio)
                for (int i = 0; i < 2; ++i)
                    if (f.tio->a[i]) f.primos.push_back(f.tio->a[i]);
        }
        return true;
    }
};

// ---------- Dibujo ----------
const float R = 24.f;

void drawEdges(sf::RenderWindow& w, Cnode* n, map<Cnode*, sf::Vector2f>& pos) {
    if (!n) return;
    for (int i = 0; i < 2; ++i) {
        if (!n->a[i]) continue;
        sf::Vertex line[] = { sf::Vertex(pos[n], sf::Color::White),
                              sf::Vertex(pos[n->a[i]], sf::Color::White) };
        w.draw(line, 2, sf::Lines);
        drawEdges(w, n->a[i], pos);
    }
}

void drawLabel(sf::RenderWindow& w, const sf::Font& font, const string& s,
               sf::Vector2f p, unsigned size, sf::Color c) {
    sf::Text t(s, font, size);
    t.setFillColor(c);
    sf::FloatRect b = t.getLocalBounds();
    t.setOrigin(b.left + b.width / 2.f, b.top + b.height / 2.f);
    t.setPosition(p);
    w.draw(t);
}

int main() {
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) { // pon un .ttf junto al ejecutable
        cerr << "Error loading font!\n";
        return 1;
    }

    const vector<int> valores = {10, 20, 30, 40, 50, 25, 11, 0, 2, 19,
                                 32, 77, 13, 69, 90, 5, 7, 14};
    size_t siguiente = 0;

    Arbol tree;
    bool haySel = false;
    int selValue = 0;

    sf::RenderWindow window(sf::VideoMode(1400, 700), "AVL Tree Visualization");
    window.setFramerateLimit(60);

    const sf::Color cSel(255, 230, 0), cAbuelo(70, 130, 255), cPadre(60, 200, 90),
                    cTio(255, 150, 40), cPrimo(200, 80, 255), cNormal(220, 50, 50);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
                haySel = false;

            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    if (siguiente < valores.size())
                        tree.Agregar(valores[siguiente++]);
                } else if (event.mouseButton.button == sf::Mouse::Right) {
                    map<Cnode*, sf::Vector2f> pos;
                    tree.layout(pos);
                    sf::Vector2f m((float)event.mouseButton.x, (float)event.mouseButton.y);
                    for (auto& [n, p] : pos) {
                        float dx = p.x - m.x, dy = p.y - m.y;
                        if (dx * dx + dy * dy <= R * R) {
                            haySel = true;
                            selValue = n->value;
                            break;
                        }
                    }
                }
            }
        }

        // Estado actual
        map<Cnode*, sf::Vector2f> pos;
        tree.layout(pos);

        Familia fam;
        bool selValida = haySel && tree.familia(selValue, fam);

        string info;
        if (selValida) {
            if (!fam.abuelo)
                info = "Nodo " + to_string(selValue) + ": no tiene abuelo, por lo tanto no tiene primos.";
            else if (fam.primos.empty())
                info = "Nodo " + to_string(selValue) + " | abuelo: " + to_string(fam.abuelo->value) +
                       " | sin primos.";
            else {
                info = "Nodo " + to_string(selValue) + " | abuelo: " + to_string(fam.abuelo->value) +
                       " | primos:";
                for (Cnode* p : fam.primos) info += " " + to_string(p->value);
            }
        } else {
            info = "Click izq: insertar siguiente (" + to_string(siguiente) + "/" +
                   to_string(valores.size()) + ")  |  Click der: seleccionar nodo  |  Esc: deseleccionar";
        }

        // Dibujo
        window.clear(sf::Color(25, 25, 30));
        drawEdges(window, tree.root(), pos);

        for (auto& [n, p] : pos) {
            sf::Color c = cNormal;
            if (selValida) {
                if (n->value == selValue)                             c = cSel;
                else if (n == fam.abuelo)                             c = cAbuelo;
                else if (n == fam.padre)                              c = cPadre;
                else if (n == fam.tio)                                c = cTio;
                else if (find(fam.primos.begin(), fam.primos.end(), n) != fam.primos.end())
                                                                      c = cPrimo;
            }
            sf::CircleShape circle(R);
            circle.setOrigin(R, R);
            circle.setPosition(p);
            circle.setFillColor(c);
            if (selValida && n->value == selValue) {
                circle.setOutlineThickness(3.f);
                circle.setOutlineColor(sf::Color::White);
            }
            window.draw(circle);
            drawLabel(window, font, to_string(n->value), p, 20, sf::Color::Black);
        }

        // Info y leyenda
        sf::Text t(info, font, 18);
        t.setFillColor(sf::Color::White);
        t.setPosition(15.f, 10.f);
        window.draw(t);

        if (selValida) {
            struct Item { const char* txt; sf::Color c; };
            Item items[] = {{"Seleccionado", cSel}, {"Abuelo", cAbuelo}, {"Padre", cPadre},
                            {"Tío", cTio}, {"Primos", cPrimo}};
            float x = 15.f;
            for (auto& it : items) {
                sf::CircleShape dot(8.f);
                dot.setFillColor(it.c);
                dot.setPosition(x, 42.f);
                window.draw(dot);
                sf::Text lt(sf::String::fromUtf8(it.txt, it.txt + string(it.txt).size()), font, 16);
                lt.setFillColor(sf::Color::White);
                lt.setPosition(x + 22.f, 40.f);
                window.draw(lt);
                x += 40.f + lt.getLocalBounds().width + 20.f;
            }
        }

        window.display();
    }
    return 0;
}

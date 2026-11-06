
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
using namespace std;

#if SFML_VERSION_MAJOR >= 3
  #define SFML3 1
#endif

struct Node {
    int value;
    Node* left;
    Node* right;
    float x, y;
    Node(int v) : value(v), left(nullptr), right(nullptr), x(0), y(0) {}
};

class SumTree {
    Node* root;

    Node* build(const vector<int>& arr, int lo, int hi) {
        int sum = 0;
        for (int i = lo; i <= hi; i++) sum += arr[i];

        Node* n = new Node(sum);
        if (lo == hi) return n;

        int mid = (lo + hi) / 2;
        n->left  = build(arr, lo, mid);
        n->right = build(arr, mid + 1, hi);
        return n;
    }

    void destroy(Node* n) {
        if (!n) return;
        destroy(n->left);
        destroy(n->right);
        delete n;
    }

    int height(Node* n) {
        if (!n) return 0;
        return 1 + max(height(n->left), height(n->right));
    }

    // Las hojas se reparten de izquierda a derecha; cada nodo interno
    // queda centrado entre sus dos hijos.
    void layout(Node* n, int depth, int& leafIdx, float dx, float dy,
                float marginX, float marginY) {
        if (!n) return;
        n->y = marginY + depth * dy;
        if (!n->left && !n->right) {
            n->x = marginX + leafIdx * dx;
            leafIdx++;
        } else {
            layout(n->left,  depth + 1, leafIdx, dx, dy, marginX, marginY);
            layout(n->right, depth + 1, leafIdx, dx, dy, marginX, marginY);
            n->x = (n->left->x + n->right->x) / 2.f;
        }
    }

public:
    SumTree() : root(nullptr) {}
    ~SumTree() { destroy(root); }

    void fromArray(const vector<int>& arr) {
        destroy(root);
        root = arr.empty() ? nullptr : build(arr, 0, (int)arr.size() - 1);
    }

    Node* getRoot() { return root; }
    int getHeight() { return height(root); }

    void computeLayout(float dx, float dy, float marginX, float marginY) {
        int leafIdx = 0;
        layout(root, 0, leafIdx, dx, dy, marginX, marginY);
    }
};

// ---------- Dibujo ----------

bool loadFont(sf::Font& font) {
    const char* paths[] = {
        "/System/Library/Fonts/Supplemental/Arial.ttf",
        "/System/Library/Fonts/Helvetica.ttc",
        "/Library/Fonts/Arial.ttf",
        "/System/Library/Fonts/Geneva.ttf"
    };
    for (const char* p : paths) {
#ifdef SFML3
        if (font.openFromFile(p)) return true;
#else
        if (font.loadFromFile(p)) return true;
#endif
    }
    return false;
}

void drawEdges(sf::RenderWindow& win, Node* n) {
    if (!n) return;
    sf::Color gray(120, 120, 120);
    for (Node* c : {n->left, n->right}) {
        if (!c) continue;
        sf::Vertex line[] = {
            sf::Vertex{sf::Vector2f(n->x, n->y), gray},
            sf::Vertex{sf::Vector2f(c->x, c->y), gray}
        };
        win.draw(line, 2, sf::PrimitiveType::Lines);
        drawEdges(win, c);
    }
}

void drawNodes(sf::RenderWindow& win, Node* n, const sf::Font& font, float radius) {
    if (!n) return;

    sf::CircleShape circle(radius);
    circle.setPointCount(60);
    circle.setFillColor(sf::Color(40, 160, 70));  // verde
    circle.setOrigin(sf::Vector2f(radius, radius));
    circle.setPosition(sf::Vector2f(n->x, n->y));
    win.draw(circle);

    string s = to_string(n->value);
#ifdef SFML3
    sf::Text text(font, s, 18);
    sf::FloatRect b = text.getLocalBounds();
    text.setOrigin(sf::Vector2f(b.position.x + b.size.x / 2.f,
                                b.position.y + b.size.y / 2.f));
#else
    sf::Text text(s, font, 18);
    sf::FloatRect b = text.getLocalBounds();
    text.setOrigin(sf::Vector2f(b.left + b.width / 2.f,
                                b.top + b.height / 2.f));
#endif
    text.setFillColor(sf::Color::White);
    text.setPosition(sf::Vector2f(n->x, n->y));
    win.draw(text);

    drawNodes(win, n->left, font, radius);
    drawNodes(win, n->right, font, radius);
}

int main() {
    vector<int> arr = {3, 5, 2, 8, 1, 4, 7};

    SumTree tree;
    tree.fromArray(arr);

    const float radius = 24.f;
    const float dx = 64.f, dy = 90.f, margin = 50.f;

    tree.computeLayout(dx, dy, margin, margin);

    unsigned width  = (unsigned)(arr.size() * dx + 2 * margin);
    unsigned height = (unsigned)(tree.getHeight() * dy + margin);

#ifdef SFML3
    sf::RenderWindow window(sf::VideoMode({width, height}), "Arbol de sumas");
#else
    sf::RenderWindow window(sf::VideoMode(width, height), "Arbol de sumas");
#endif
    window.setFramerateLimit(60);

    sf::Font font;
    if (!loadFont(font)) {
        cerr << "No se pudo cargar ninguna fuente\n";
        return 1;
    }

    while (window.isOpen()) {
#ifdef SFML3
        while (const optional<sf::Event> ev = window.pollEvent()) {
            if (ev->is<sf::Event::Closed>()) window.close();
        }
#else
        sf::Event ev;
        while (window.pollEvent(ev)) {
            if (ev.type == sf::Event::Closed) window.close();
        }
#endif
        window.clear(sf::Color(30, 30, 35));
        drawEdges(window, tree.getRoot());
        drawNodes(window, tree.getRoot(), font, radius);
        window.display();
    }
    return 0;
}

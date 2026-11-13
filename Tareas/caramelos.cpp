#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <optional>
#include <algorithm>
#include <cmath>
using namespace std;

#if SFML_VERSION_MAJOR >= 3
  #define SFML3 1
#endif

const int NIL = -1;  // marca de "no hay nodo" en el arreglo level-order

struct Node {
    int candies;          // candies actuales (cambian durante la simulacion)
    int initial;          // candies iniciales (para poder reiniciar)
    int excess = 0;       // candies del subtree - nodos del subtree
    int pendingUp = 0;    // candies que aun deben subir  por la arista (this -> parent)
    int pendingDown = 0;  // candies que aun deben bajar  por la arista (parent -> this)
    Node* left = nullptr;
    Node* right = nullptr;
    Node* parent = nullptr;
    float x = 0, y = 0;
    Node(int c) : candies(c), initial(c) {}
};

class CandyTree {
    Node* root = nullptr;
    vector<Node*> nodes;
    int totalMoves = 0;

    Node* make(int v) {
        Node* n = new Node(v);
        nodes.push_back(n);
        return n;
    }

    // Post-order: el exceso de un nodo depende del exceso de sus hijos.
    int computeExcess(Node* n) {
        if (!n) return 0;
        int l = computeExcess(n->left);
        int r = computeExcess(n->right);
        n->excess = n->initial + l + r - 1;
        return n->excess;
    }

    int height(Node* n) {
        if (!n) return 0;
        return 1 + max(height(n->left), height(n->right));
    }

    // Las hojas se reparten de izquierda a derecha; un nodo interno queda
    // centrado entre sus hijos (o justo encima si solo tiene uno).
    void layout(Node* n, int depth, int& leafIdx, float dx, float dy,
                float marginX, float marginY) {
        if (!n) return;
        n->y = marginY + depth * dy;
        if (!n->left && !n->right) {
            n->x = marginX + leafIdx * dx;
            leafIdx++;
            return;
        }
        layout(n->left,  depth + 1, leafIdx, dx, dy, marginX, marginY);
        layout(n->right, depth + 1, leafIdx, dx, dy, marginX, marginY);
        if (n->left && n->right) n->x = (n->left->x + n->right->x) / 2.f;
        else n->x = (n->left ? n->left : n->right)->x;
    }

public:
    ~CandyTree() { for (Node* n : nodes) delete n; }

    // Formato level-order como en GeeksforGeeks, con -1 para "sin nodo".
    void fromLevelOrder(const vector<int>& a) {
        for (Node* n : nodes) delete n;
        nodes.clear();
        root = nullptr;
        if (a.empty() || a[0] == NIL) return;

        root = make(a[0]);
        queue<Node*> q;
        q.push(root);
        size_t i = 1;
        while (!q.empty() && i < a.size()) {
            Node* cur = q.front(); q.pop();
            if (i < a.size() && a[i] != NIL) {
                cur->left = make(a[i]);
                cur->left->parent = cur;
                q.push(cur->left);
            }
            i++;
            if (i < a.size() && a[i] != NIL) {
                cur->right = make(a[i]);
                cur->right->parent = cur;
                q.push(cur->right);
            }
            i++;
        }
        computeExcess(root);
        reset();
    }

    // El problema solo tiene solucion si total de candies == numero de nodos.
    bool isValid() const {
        int sum = 0;
        for (Node* n : nodes) sum += n->initial;
        return !nodes.empty() && sum == (int)nodes.size();
    }

    // Deja el arbol como al inicio y calcula cuantos candies deben cruzar
    // cada arista: |exceso| del hijo, hacia arriba si sobra, hacia abajo si falta.
    void reset() {
        totalMoves = 0;
        for (Node* n : nodes) {
            n->candies = n->initial;
            n->pendingUp = n->pendingDown = 0;
            if (n->parent) {
                if (n->excess > 0) n->pendingUp = n->excess;
                else if (n->excess < 0) n->pendingDown = -n->excess;
                totalMoves += abs(n->excess);
            }
        }
    }

    // Busca UN move valido: una arista que todavia tenga candies por cruzar
    // y cuyo emisor tenga al menos 1 candy ahora mismo. Lo descuenta del emisor
    // (el receptor lo recibe cuando termina la animacion).
    bool startMove(Node*& from, Node*& to) {
        for (Node* n : nodes) {
            if (!n->parent) continue;
            if (n->pendingUp > 0 && n->candies > 0) {
                n->pendingUp--;
                n->candies--;
                from = n; to = n->parent;
                return true;
            }
            if (n->pendingDown > 0 && n->parent->candies > 0) {
                n->pendingDown--;
                n->parent->candies--;
                from = n->parent; to = n;
                return true;
            }
        }
        return false;
    }

    const vector<Node*>& getNodes() const { return nodes; }
    int getTotalMoves() const { return totalMoves; }
    int getHeight() { return height(root); }

    int countLeaves() const {
        int c = 0;
        for (Node* n : nodes) if (!n->left && !n->right) c++;
        return c;
    }

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
        "/System/Library/Fonts/Geneva.ttf",
        "/usr/share/fonts/TTF/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/liberation/LiberationSans-Regular.ttf"
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

void drawLabel(sf::RenderWindow& win, const sf::Font& font, const string& s,
               unsigned size, float x, float y, sf::Color color, bool center) {
#ifdef SFML3
    sf::Text text(font, s, size);
    sf::FloatRect b = text.getLocalBounds();
    if (center)
        text.setOrigin(sf::Vector2f(b.position.x + b.size.x / 2.f,
                                    b.position.y + b.size.y / 2.f));
#else
    sf::Text text(s, font, size);
    sf::FloatRect b = text.getLocalBounds();
    if (center)
        text.setOrigin(sf::Vector2f(b.left + b.width / 2.f,
                                    b.top + b.height / 2.f));
#endif
    text.setFillColor(color);
    text.setPosition(sf::Vector2f(x, y));
    win.draw(text);
}

void drawEdges(sf::RenderWindow& win, const CandyTree& tree, const sf::Font& font) {
    sf::Color gray(120, 120, 120);
    for (Node* c : tree.getNodes()) {
        if (!c->parent) continue;
        Node* p = c->parent;
        sf::Vertex line[] = {
            sf::Vertex{sf::Vector2f(p->x, p->y), gray},
            sf::Vertex{sf::Vector2f(c->x, c->y), gray}
        };
        win.draw(line, 2, sf::PrimitiveType::Lines);

        // Candies que todavia deben cruzar esta arista
        string label;
        if (c->pendingUp > 0)   label = "sube " + to_string(c->pendingUp);
        if (c->pendingDown > 0) label = "baja " + to_string(c->pendingDown);
        if (!label.empty())
            drawLabel(win, font, label, 15, (p->x + c->x) / 2.f + 34.f,
                      (p->y + c->y) / 2.f, sf::Color(255, 220, 120), true);
    }
}

void drawNodes(sf::RenderWindow& win, const CandyTree& tree, const sf::Font& font,
               float radius) {
    for (Node* n : tree.getNodes()) {
        sf::CircleShape circle(radius);
        circle.setPointCount(60);
        if (n->candies == 1)      circle.setFillColor(sf::Color(40, 160, 70));   // listo
        else if (n->candies == 0) circle.setFillColor(sf::Color(190, 60, 60));   // le falta
        else                      circle.setFillColor(sf::Color(220, 150, 40));  // le sobra
        circle.setOrigin(sf::Vector2f(radius, radius));
        circle.setPosition(sf::Vector2f(n->x, n->y));
        win.draw(circle);

        drawLabel(win, font, to_string(n->candies), 18, n->x, n->y,
                  sf::Color::White, true);

        // Exceso del subtree (calculado por el DFS, no cambia durante la animacion)
        string e = string("e=") + (n->excess > 0 ? "+" : "") + to_string(n->excess);
        drawLabel(win, font, e, 14, n->x + radius + 4.f, n->y - radius,
                  sf::Color(140, 170, 220), false);
    }
}

int main() {
    // Arbol en formato level-order (-1 = sin nodo):
    //        3
    //       / \
    //      0   0
    //         / \
    //        2   0
    vector<int> arr = {3, 0, 0, NIL, NIL, 2, 0};

    CandyTree tree;
    tree.fromLevelOrder(arr);
    if (!tree.isValid()) {
        cerr << "La suma de candies debe ser igual al numero de nodos\n";
        return 1;
    }

    const float radius = 24.f;
    const float dx = 80.f, dy = 90.f, margin = 50.f, top = 100.f;
    const float duration = 0.5f;  // segundos por move

    int leaves = tree.countLeaves();
    float treeW = (leaves - 1) * dx;
    unsigned width  = (unsigned)max(640.f, treeW + 2 * margin + 80.f);
    unsigned height = (unsigned)(top + (tree.getHeight() - 1) * dy + margin + 20.f);
    tree.computeLayout(dx, dy, (width - treeW) / 2.f, top);

#ifdef SFML3
    sf::RenderWindow window(sf::VideoMode({width, height}), "Distribute Candies");
#else
    sf::RenderWindow window(sf::VideoMode(width, height), "Distribute Candies");
#endif
    window.setFramerateLimit(60);

    sf::Font font;
    if (!loadFont(font)) {
        cerr << "No se pudo cargar ninguna fuente\n";
        return 1;
    }

    Node *from = nullptr, *to = nullptr;
    bool animating = false, autoplay = false;
    int moves = 0;
    sf::Clock clock;

    while (window.isOpen()) {
        bool clicked = false, doReset = false;

#ifdef SFML3
        while (const optional<sf::Event> ev = window.pollEvent()) {
            if (ev->is<sf::Event::Closed>()) window.close();
            else if (const auto* mb = ev->getIf<sf::Event::MouseButtonPressed>()) {
                if (mb->button == sf::Mouse::Button::Left) clicked = true;
            } else if (const auto* kp = ev->getIf<sf::Event::KeyPressed>()) {
                if (kp->code == sf::Keyboard::Key::Space) clicked = true;
                if (kp->code == sf::Keyboard::Key::A) autoplay = !autoplay;
                if (kp->code == sf::Keyboard::Key::R) doReset = true;
            }
        }
#else
        sf::Event ev;
        while (window.pollEvent(ev)) {
            if (ev.type == sf::Event::Closed) window.close();
            else if (ev.type == sf::Event::MouseButtonPressed &&
                     ev.mouseButton.button == sf::Mouse::Left) clicked = true;
            else if (ev.type == sf::Event::KeyPressed) {
                if (ev.key.code == sf::Keyboard::Space) clicked = true;
                if (ev.key.code == sf::Keyboard::A) autoplay = !autoplay;
                if (ev.key.code == sf::Keyboard::R) doReset = true;
            }
        }
#endif

        if (doReset) {
            tree.reset();
            animating = false;
            moves = 0;
        }

        // Avance de la animacion: al llegar, el receptor recibe el candy
        float t = 0.f;
        if (animating) {
            t = clock.getElapsedTime().asSeconds() / duration;
            if (t >= 1.f) {
                to->candies++;
                moves++;
                animating = false;
            }
        }

        // Siguiente move (click, espacio o autoplay)
        if (!animating && (clicked || autoplay)) {
            if (tree.startMove(from, to)) {
                animating = true;
                clock.restart();
                t = 0.f;
            }
        }

        bool finished = !animating && moves == tree.getTotalMoves();

        window.clear(sf::Color(30, 30, 35));
        drawEdges(window, tree, font);
        drawNodes(window, tree, font, radius);

        if (animating) {
            float tt = min(t, 1.f);
            sf::CircleShape candy(9.f);
            candy.setFillColor(sf::Color(255, 210, 60));
            candy.setOutlineColor(sf::Color(200, 60, 120));
            candy.setOutlineThickness(2.f);
            candy.setOrigin(sf::Vector2f(9.f, 9.f));
            candy.setPosition(sf::Vector2f(from->x + (to->x - from->x) * tt,
                                           from->y + (to->y - from->y) * tt));
            window.draw(candy);
        }

        string head = "Moves: " + to_string(moves) + " / " + to_string(tree.getTotalMoves());
        if (finished) head += "  - listo, todos los nodos tienen 1 candy";
        drawLabel(window, font, head, 20, 16.f, 12.f,
                  finished ? sf::Color(90, 220, 120) : sf::Color::White, false);
        drawLabel(window, font, "Click / Espacio: siguiente move    A: auto    R: reiniciar",
                  14, 16.f, 44.f, sf::Color(170, 170, 170), false);
        drawLabel(window, font, "e = candies del subtree - nodos del subtree",
                  14, 16.f, 64.f, sf::Color(140, 170, 220), false);

        window.display();
    }
    return 0;
}

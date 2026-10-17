#include <iostream>
using namespace std;

class CDeque {
private:

    int** map;
    int chunk_size, map_size;
    int n_element;
    int start_chunk, end_chunk;
    int start_offset, end_offset;

public:

    CDeque(int cs, int ms) {
        chunk_size = cs;
        map_size = ms;

        map = new int* [map_size]();

        start_chunk = end_chunk = map_size / 2;

        map[start_chunk] = new int[chunk_size];
        start_offset = end_offset = chunk_size / 2;

        n_element = 0;
    }

    ~CDeque() {
        for (int i = start_chunk; i <= end_chunk; ++i)
            delete[] map[i];
        delete[] map;
    };

    void push_back(int x) {

        if (end_offset == chunk_size) {
            if (end_chunk + 1 == map_size) {
                expand_map();
            }
            end_chunk++;

            if (map[end_chunk] == nullptr) {
                map[end_chunk] = new int[chunk_size];
            }

            end_offset = 0;
        }

        map[end_chunk][end_offset] = x;
        end_offset++;
        n_element++;

    };

    void push_front(int x) {
        if (start_offset == 0) {
            if (start_chunk == 0) {
                expand_map();
            }
            start_chunk--;
            if (map[start_chunk] == nullptr) {
                map[start_chunk] = new int[chunk_size];
            }
            start_offset = chunk_size;
        }
        start_offset--;
        map[start_chunk][start_offset] = x;
        n_element++;
    }
    
    void pop_front() {

        if (n_element == 0) return;
        n_element--;
        if (n_element == 0) {
            start_offset = end_offset = chunk_size / 2;
            return;
        }

        start_offset++;
        if (start_offset == chunk_size) {
            delete[] map[start_chunk];
            map[start_chunk] = nullptr;
            start_chunk++;
            start_offset = 0;
        }
    }

    void pop_back() {

        if (n_element == 0) return;

        n_element--;

        if (n_element == 0) {
            start_offset = end_offset = chunk_size / 2;
            return;
        }

        end_offset--;

        if (end_offset == 0) {
            delete[] map[end_chunk];
            map[end_chunk] = nullptr;
            end_chunk--;
            end_offset = chunk_size;
        }
    }

    int& front() {
        return map[start_chunk][start_offset];
    }

    int& back() {
        return map[end_chunk][end_offset-1];
    }
    

    int& operator[](int i) {

        int pos = start_offset + i;
        int c = start_chunk + pos / chunk_size;
        int o = pos % chunk_size;

        return map[c][o];
    }

    void print() {

        int c = start_chunk;
        int o = start_offset;

        for (int i = 0; i < n_element; i++) {
            cout << map[c][o]<< ", ";

            o++;

            if (o == chunk_size) {
                c++;
                o = 0;
            }

        }
        cout << endl;

    };

    void expand_map() {
        int n_tam = map_size * 2;
        int** n_map = new int* [n_tam] ();
        int espacio_libre = map_size / 2;

        for (int j = 0; j < map_size; j++ ) {
            n_map[espacio_libre + j] = map[j];
        }

        start_chunk = start_chunk + espacio_libre;
        end_chunk = end_chunk + espacio_libre;

        delete[] map;
        map = n_map;
        map_size = n_tam;
    };
};


#include <deque>
#include <cassert>
#include <cstdlib>

int main() {
    CDeque a(3, 4);
    std::deque<int> b;

    for (int k = 0; k < 100000; k++) {
        int op = rand() % 4;
        int x = rand();
        if (op == 0) { a.push_back(x);  b.push_back(x); }
        if (op == 1) { a.push_front(x); b.push_front(x); }
        if (op == 2 && !b.empty()) { a.pop_back();  b.pop_back(); }
        if (op == 3 && !b.empty()) { a.pop_front(); b.pop_front(); }

        if (!b.empty()) {
            assert(a.front() == b.front());
            assert(a.back() == b.back());
            assert(a[b.size() / 2] == b[b.size() / 2]);
        }
    }
}

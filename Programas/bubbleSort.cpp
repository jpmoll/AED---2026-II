#include <iostream>
#include <chrono>
#include <ctime>
#include <algorithm>
using namespace std;
using namespace std::chrono;

void print(int a[], int n) {
    for (int i = 0; i < n; i++) {
        cout << a[i] << " ";
    }
    cout << endl;
}

void bubbleSortComp(int* arr, int n) {
    for (int i = 0; i < n - 1; ++i) {
        for (int j = 0; j < n - i - 1; ++j) {
            if (*(arr + j) > *(arr + j + 1)) {
                int temp = *(arr + j);
                *(arr + j) = *(arr + j + 1);
                *(arr + j + 1) = temp;
            }
        }
    }
}

class cmp {
public:
    virtual bool compare(int a, int b) = 0;
};

class CLess : public cmp {
public:
    bool compare(int a, int b) override {
        return a < b;
    }
};

class CGreater : public cmp {
public:
    bool compare(int a, int b) override {
        return a > b;
    }
};

void bubbleSortHerencia(int arr[], int n, cmp *c) {
    for (int i = 0; i < n - 1; ++i) {
        for (int j = 0; j < n - i - 1; ++j) {
            if (c->compare(arr[j], arr[j + 1])) {
                int temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

template <class T>
struct CLessf {
    bool cmp(T a, T b) {
        return a < b;
    }
};

template <class T>
struct CGreaterf {
    bool cmp(T a, T b) {
        return a > b;
    }
};

template <class T>
void bubbleSortFunctores(int arr[], int n, T c) {
    for (int i = 0; i < n - 1; ++i) {
        for (int j = 0; j < n - i - 1; ++j) {
            if (c.cmp(arr[j], arr[j + 1])) {
                int temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

template <class T>
struct CLessfI {
    inline bool cmp(T a, T b) {
        return a < b;
    }
};

template <class T>
struct CGreaterfI {
    inline bool cmp(T a, T b) {
        return a > b;
    }
};

template <class T>
void bubbleSortFunctoresInline(int arr[], int n, T c) {
    for (int i = 0; i < n - 1; ++i) {
        for (int j = 0; j < n - i - 1; ++j) {
            if (c.cmp(arr[j], arr[j + 1])) {
                int temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

void fillRandom(int arr[], int n) {
    srand(time(nullptr));
    for (int i = 0; i < n; ++i) {
        arr[i] = rand() % 10000 + 1; // números aleatorios entre 1 y 10000
    }
}

int main() {
    int arr[10000];
    fillRandom(arr, 10000);

    int arr2[10000], arr3[10000], arr4[10000], arr5[10000], arr6[10000], arr7[10000];
    copy(arr, arr + 10000, arr2);
    copy(arr, arr + 10000, arr3);
    copy(arr, arr + 10000, arr4);
    copy(arr, arr + 10000, arr5);
    copy(arr, arr + 10000, arr6);
    copy(arr, arr + 10000, arr7);

    auto start = high_resolution_clock::now();
    bubbleSortComp(arr, 10000);
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);
    cout << "Tiempo de ejecución para bubbleSortComp: " << duration.count() << " milisegundos" << endl;

    start = high_resolution_clock::now();
    bubbleSortHerencia(arr2, 10000, new CLess());
    stop = high_resolution_clock::now();
    duration = duration_cast<milliseconds>(stop - start);
    cout << "Tiempo de ejecución para bubbleSortHerencia con CLess: " << duration.count() << " milisegundos" << endl;

    start = high_resolution_clock::now();
    bubbleSortHerencia(arr3, 10000, new CGreater());
    stop = high_resolution_clock::now();
    duration = duration_cast<milliseconds>(stop - start);
    cout << "Tiempo de ejecución para bubbleSortHerencia con CGreater: " << duration.count() << " milisegundos" << endl;

    start = high_resolution_clock::now();
    bubbleSortFunctores(arr4, 10000, CLessf<int>());
    stop = high_resolution_clock::now();
    duration = duration_cast<milliseconds>(stop - start);
    cout << "Tiempo de ejecución para bubbleSortFunctores con CLessf: " << duration.count() << " milisegundos" << endl;

    start = high_resolution_clock::now();
    bubbleSortFunctores(arr5, 10000, CGreaterf<int>());
    stop = high_resolution_clock::now();
    duration = duration_cast<milliseconds>(stop - start);
    cout << "Tiempo de ejecución para bubbleSortFunctores con CGreaterf: " << duration.count() << " milisegundos" << endl;

    start = high_resolution_clock::now();
    bubbleSortFunctoresInline(arr6, 10000, CLessfI<int>());
    stop = high_resolution_clock::now();
    duration = duration_cast<milliseconds>(stop - start);
    cout << "Tiempo de ejecución para bubbleSortFunctoresInline con CLessfI: " << duration.count() << " milisegundos" << endl;

    start = high_resolution_clock::now();
    bubbleSortFunctoresInline(arr7, 10000, CGreaterfI<int>());
    stop = high_resolution_clock::now();
    duration = duration_cast<milliseconds>(stop - start);
    cout << "Tiempo de ejecución para bubbleSortFunctoresInline con CGreaterfI: " << duration.count() << " milisegundos" << endl;

    return 0;
}

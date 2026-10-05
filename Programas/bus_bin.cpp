#include <iostream>
using namespace std;

int * mid(int * a, int * b){
    return a + (b - a) /2;
}

bool bin_bus(int *ini, int *fin, int *& pos, int n){
    while (ini <= fin) {
        int *medio = mid(ini, fin);
        if (*medio == n) {
            pos = medio; 
            cout << "Se encontro el valor" << endl;
            return true;
        } else if (*medio < n) {
            ini = medio + 1; 
        } else {
            fin = medio - 1; 
        }
    }
    cout << "No se encontro el valor " << n << endl;
    return false; 
}

int main(){
    int a[13] = {1, 3, 5, 7, 10 ,13, 26, 28, 30, 60, 128, 558, 653};
    int *pos = NULL;
    bin_bus(a, a + 12, pos, 10);
    bin_bus(a, a + 12, pos,60);
    bin_bus(a, a + 12, pos, 0);
    bin_bus(a, a + 12, pos, 322);
    return 0;
}
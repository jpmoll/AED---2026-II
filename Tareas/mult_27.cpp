#include <iostream>
using namespace std;

int main() {
    char partial[20];
    cout << "Numero (sin el ultimo digito) ";
    cin >> partial;

    for (int d = 0; d <= 9; d++) {
        char candidato_str[21];
        char* p = candidato_str;
        char* q = partial;

        while (*q != '\0') {
            *p = *q;
            p++;
            q++;
        }
        *p = '0' + d;
        p++;
        *p = '\0';

        // conversor
      
        long long candidato = 0;
        char* r = candidato_str;
        while (*r != '\0') {
            candidato = candidato * 10 + (*r - '0');
            r++;
        }

        if (candidato % 27 == 0) {
            long long original = candidato / 27;
            cout << candidato << "\n";
            cout << "multiplicador " << original << "\n";
            return 0;
        }
    }

    cout << "El numero no es múltipo de 27.\n";
    return 0;
}

#include <iostream>
#include <sstream>
#include <vector>
#include <cctype>
#include <stack>

using namespace std;

vector<string> term = {"suma", "total", "resta"};

//Verificar numero valido
bool numero(const string &s) {              
    for (char c : s) {
        if (!isdigit(c)) return false;
    }
    return !s.empty();
}

//Verificar ident valido
bool ident(const string &s) {
    for (const string &id : term) {
        if (s == id) return true;
    }
    return false;
}

//Verificar factor valido
bool factor(const string &token) {
    return numero(token) || ident(token) || token == "(" || token == ")";
}

//Verificar toda la expresion
bool verifcar_expr(string expr) {
    istringstream tokens(expr);
    string token;
    vector<string> vToken;
    stack<char> parentesis;
    
    while (tokens >> token) {
        vToken.push_back(token);
    }
    
    if (vToken.empty()) return false;
    
    for (size_t i = 0; i < vToken.size(); i++) {
        if (vToken[i] == "(") {
            parentesis.push('(');
        } else if (vToken[i] == ")") {
            if (parentesis.empty()) 
                return false;
            parentesis.pop();
        } else if (vToken[i] == "+" || vToken[i] == "-" ||vToken[i] == "*" || vToken[i] == "/") {
            if (i == 0 || i == vToken.size() - 1) 
                return false;
            if (!(factor(vToken[i - 1]) && factor(vToken[i + 1]))) 
                return false;
        }
    }
    
    return parentesis.empty();
}

int main() {
    string expr1 = "( suma + 4 ) / total";
    string expr2 = "4 * resta + 3";
    
    cout << "Expresion 1 " << expr1 << (verifcar_expr(expr1) ? " --- pertenece" : " --- no pertenece") << endl;
    cout << "Expresion 2 '" << expr1 << (verifcar_expr(expr2) ? " --- pertenece" : " --- nopertenece") << endl;
    
    return 0;
}
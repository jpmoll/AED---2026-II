#include <iostream>
#include <assert.h>
#include <string>
#include <cctype>
#include <stdexcept>

using namespace std;

template <typename T>
class CVector {

public:

    CVector(int s = 4);
    CVector(const CVector<T>& other);
    CVector<T>& operator=(const CVector<T>& other);
    ~CVector();
    void push_front(const T& x);
    void pop_front();
    void push_back(const T& x);
    void pop_back();
    void expand();
    void collapse();
    T& operator[](int i);
    T& front();
    T& back();
    bool empty() const;
    int  count() const;
    void print();

    // Acceso a punteros al inicio/fin, para recorrer sin usar indices
    T* begin() { return vec; }
    T* end() { return vec + nelem; }
    const T* begin() const { return vec; }
    const T* end() const { return vec + nelem; }

private:
    T* vec;
    int nelem, size;
};

template <typename T>
CVector<T>::CVector(int s) {

    size = s;
    vec = new T[size];
    nelem = 0;
}

template <typename T>
CVector<T>::CVector(const CVector<T>& other){

    size = other.size;
    nelem = other.nelem;
    vec = new T[size];

    T* p = vec;
    const T* q = other.vec;
    while (q != other.vec + nelem)
        *p++ = *q++;
}

template <typename T>
CVector<T>& CVector<T>::operator=(const CVector<T>& other){

    if (this == &other)
        return *this;
    delete[] vec;
    size = other.size;
    nelem = other.nelem;
    vec = new T[size];

    T* p = vec;
    const T* q = other.vec;
    while (q != other.vec + nelem)
        *p++ = *q++;

    return *this;
}

template <typename T>
CVector<T>::~CVector(){

    delete[] vec;
}

template <typename T>
void CVector<T>::expand(){

    T *q = new T[size * 2];

    T* src = vec;
    T* dst = q;
    while (src != vec + size)
        *dst++ = *src++;

    delete[] vec;
    vec = q;
    size *= 2;
}

template <typename T>
void CVector<T>::collapse()
{}

template <typename T>
void CVector<T>::push_front(const T& x)
{
    if (nelem == size)
        expand();

    for (T* p = vec + nelem; p > vec; --p)
        *p = *(p - 1);

    *vec = x;
    nelem++;
}

template <typename T>
void CVector<T>::pop_front()
{
    for (T* p = vec; p < vec + nelem - 1; ++p)
        *p = *(p + 1);

    nelem--;
}

template <typename T>
void CVector<T>::push_back(const T& x)
{
    if (nelem == size)
        expand();
    *(vec + nelem) = x;
    nelem++;
}

template <typename T>
void CVector<T>::pop_back()
{
    nelem--;
}

template <typename T>
T& CVector<T>::operator[](int i)
{
    assert(i < size);
    return *(vec + i);
}

template <typename T>
T& CVector<T>::front()
{
    return *vec;
}

template <typename T>
T& CVector<T>::back()
{
    return *(vec + nelem - 1);
}

template <typename T>
bool CVector<T>::empty() const
{
    return nelem == 0;
}

template <typename T>
int CVector<T>::count() const
{
    return nelem;
}

template <typename T>
void CVector<T>::print()
{
    for (T* p = vec; p != vec + nelem; ++p)
        cout << *p << " ";
}

// String a números/operadores/paréntesis

CVector<string> tokenize(const string& expr)
{
    CVector<string> tokens;

    const char* p = expr.c_str();
    const char* end = p + expr.size();

    while (p < end)
    {
        char c = *p;

        if (isspace((unsigned char)c)) { ++p; continue; }

        if (isdigit((unsigned char)c) || c == '.')
        {
            string num;
            while (p < end && (isdigit((unsigned char)*p) || *p == '.'))
                num += *p++;
            tokens.push_back(num);
            continue;
        }

        if (c == '+' || c == '-' || c == '*' || c == '/' ||
            c == '(' || c == ')')
        {
            tokens.push_back(string(1, c));
            ++p;
            continue;
        }

        throw runtime_error(string("Caracter invalido: ") + c);
    }
    return tokens;
}

//  Infijo a postfijo

int precedence(const string& op)
{
    if (op == "+" || op == "-") return 1;
    if (op == "*" || op == "/") return 2;
    return 0;
}

bool isOperator(const string& tok) {
    return tok == "+" || tok == "-" || tok == "*" || tok == "/";
}

CVector<string> infixToPostfix(const CVector<string>& tokensIn) {

    CVector<string> tokens = tokensIn;
    CVector<string> output;
    CVector<char> ops;

    for (string* pt = tokens.begin(); pt != tokens.end(); ++pt){

        string tok = *pt;

        if (isOperator(tok))
        {
            while (!ops.empty() && ops.back() != '(' &&
                   precedence(string(1, ops.back())) >= precedence(tok))
            {
                output.push_back(string(1, ops.back()));
                ops.pop_back();
            }
            ops.push_back(tok[0]);
        }
        else if (tok == "(")
        {
            ops.push_back('(');
        }
        else if (tok == ")")
        {
            while (!ops.empty() && ops.back() != '(')
            {
                output.push_back(string(1, ops.back()));
                ops.pop_back();
            }
            if (ops.empty())
                throw runtime_error("Parentesis desbalanceados");
            ops.pop_back();
        }
        else
        {
            output.push_back(tok);
        }
    }

    while (!ops.empty())
    {
        if (ops.back() == '(')
            throw runtime_error("Parentesis desbalanceados");
        output.push_back(string(1, ops.back()));
        ops.pop_back();
    }

    return output;
}

//  Evaluación de la expresion postfija usando CVector<double> como pila

double evaluatePostfix(CVector<string> postfix)
{
    CVector<double> stack;

    for (string* pt = postfix.begin(); pt != postfix.end(); ++pt)
    {
        string tok = *pt;

        if (isOperator(tok))
        {
            if (stack.count() < 2)
                throw runtime_error("Expresion invalida");

            double b = stack.back(); stack.pop_back();
            double a = stack.back(); stack.pop_back();
            double r = 0;

            if (tok == "+") r = a + b;
            else if (tok == "-") r = a - b;
            else if (tok == "*") r = a * b;
            else if (tok == "/")
            {
                if (b == 0) throw runtime_error("Division por cero");
                r = a / b;
            }
            stack.push_back(r);
        }
        else
        {
            stack.push_back(stod(tok));
        }
    }

    if (stack.count() != 1)
        throw runtime_error("Expresion invalida");

    return stack.back();
}

int main(){

    string expr = "((4+3)*9)+15/5";

    CVector<string> tokens = tokenize(expr);
    cout << "Expresion infija: " << expr << "\n";

    cout << "Tokens: ";
    tokens.print();
    cout << "\n";

    CVector<string> postfix = infixToPostfix(tokens);
    cout << "Forma postfija: ";
    postfix.print();
    cout << "\n";

    double result = evaluatePostfix(postfix);
    cout << "Resultado: " << result << "\n";

    return 0;
}

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
    for (int i = 0; i < nelem; ++i)
        vec[i] = other.vec[i];
}

template <typename T>
CVector<T>& CVector<T>::operator=(const CVector<T>& other){
    
    if (this == &other)
        return *this;
    delete[] vec;
    size = other.size;
    nelem = other.nelem;
    vec = new T[size];
    for (int i = 0; i < nelem; ++i)
        vec[i] = other.vec[i];
    return *this;
}

template <typename T>
CVector<T>::~CVector(){
    
    delete[] vec;
}

template <typename T>
void CVector<T>::expand(){
    
    T *q = new T[size * 2];
    for (int i = 0; i < size; ++i)
        q[i] = vec[i];
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
    for (int i = nelem; i > 0; i--)
        vec[i] = vec[i - 1];
    vec[0] = x;
    nelem++;
}

template <typename T>
void CVector<T>::pop_front()
{
    for (int i = 0; i < nelem - 1; ++i)
        vec[i] = vec[i + 1];
    nelem--;
}

template <typename T>
void CVector<T>::push_back(const T& x)
{
    if (nelem == size)
        expand();
    vec[nelem] = x;
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
    for (int i = 0; i < nelem; ++i)
        cout << vec[i] << " ";
}

// String a números/operadores/paréntesis


CVector<string> tokenize(const string& expr)
{
    CVector<string> tokens;
    
    int i = 0;
    int n = expr.size();

    while (i < n)
    {
        char c = expr[i];

        if (isspace(c)) { i++; continue; }

        if (isdigit(c) || c == '.')
        {
            string num;
            while (i < n && (isdigit(expr[i]) || expr[i] == '.'))
                num += expr[i++];
            tokens.push_back(num);
            continue;
        }

        if (c == '+' || c == '-' || c == '*' || c == '/' ||
            c == '(' || c == ')')
        {
            tokens.push_back(string(1, c));
            i++;
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
  
    for (int i = 0; i < tokens.count(); ++i){
        
        string tok = tokens[i];

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

    for (int i = 0; i < postfix.count(); ++i)
    {
        string tok = postfix[i];

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

#include <iostream>

using namespace std;

struct StackNode
{
    int Stack[5];
    int* end = Stack + 4;
    StackNode* next=nullptr;
};

struct StackDeq
{
    int* top = nullptr;
    StackNode* topnode = nullptr;
    
    void push(int x);
    bool pop(int &x);
};

void StackDeq::push(int x)
{
   //TO DO
}

bool StackDeq::pop(int& x)
{
    //TO DO
}

int main()
{
    StackDeq sd;
    int dev;

    cout << "ingresamos 13 elementos: ";
    for (int i = 1; i <= 13; i++)
    {
        sd.push(i);
        cout << i << " ";
    }
        
    cout <<endl<< "Retiramos 5 elementos: ";
    for (int i = 0; i < 5; i++)
    {
        sd.pop(dev);
        cout << dev << " ";
    }

    cout <<endl<<"ingresamos 10 elementos: ";
    for (int i = 20; i <= 29; i++)
    {
        sd.push(i);
        cout << i << " ";
    }

    cout << endl <<"retiramos elementos hasta que la cola se vacie: ";
    while (sd.pop(dev))
        cout << dev << " ";
    
}
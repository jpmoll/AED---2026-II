#include <iostream>
#include <deque>

template <class T>
class CMaxHeap3
{
public:
    void push(int n);
    int top();
    void pop();
    void print();
private:
    std::deque<T> heap;
};

template <class T>
int CMaxHeap3<T>::CMaxHeap3::top()
{
    return 0;
}

template <class T>
void CMaxHeap3<T>::CMaxHeap3::push(int x)
{
}

template <class T>
void CMaxHeap3<T>::CMaxHeap3::pop()
{
}

template <class T>
void CMaxHeap3<T>:: CMaxHeap3::print()
{
    for ( auto i = heap.begin(); i != heap.end(); ++i )
        std::cout<<*i<<" ";
    std::cout<<"\n";
}

int main()
{
    CMaxHeap3<int> h;
    std::vector<int> vpush = {30,35,40,50,43,36,60,51,70,90,66,77,23,21,49,88,73};
    for (auto i : vpush)
        h.push(i);
    h.print();
    for (int j = 0; j < 5; j++)
        h.pop();
    h.print();
}

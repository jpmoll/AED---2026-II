
#include <iostream>
#include <thread>

using namespace std;

void F1(char c, int n)
{
    for ( int i = 0; i < n; i++ )
        cout<<c;
}

int main()
{
    thread t(F1,'c',50);
    
    t.join();
    
    cout<<"\n";
}

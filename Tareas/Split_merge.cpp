
#include <iostream>
using namespace std;

struct UTIL {
    void split(int* ini, int* fin);
    void merge(int* ini, int* mid, int* fin);
};

class div3 {
    bool operator()(int a) {
        return !(a % 3);
    }
};

int main()
{
    int arr1[] = { 1,4,8,7,21,3 ,4,5,6,9 };
    int arr2[] = { 3,2,1,5,9,7,11,8,33,25,4 };

    //  3, 9, 33,  2, 1, 5, 7, 11, 8, 25, 4
    // - cumple -   ------ no cumple ------

    //  1,4,8,7,21,3,4,5,6,9
    //  ^          ^       ^
    // ini        mid     fin
    // 
    //  1,4,8,7,21,3     p > q - Falso  p++
    //  p          q
    // 
    //  1,4,8,7,21,3     p > q - True   Swap
    //    p        q
    // 
    //      r
    //  1,3,8,7,21,4
    //    p        q
    // 
    //    
    //  1,3,4,8,7,21
    //      p      q
    // 
    //  1,3,4,8,7,21     q++
    //            p=q
    // 
    //  1,3,4,8,7,21,4
    //  p            q 
    // 
    // 
    //    r ->           r recorre los elementos a la derecha despues de un swap 
    //  1,3,4,8,7,21
    //  p ->      q->    p avanza hasta llegar a q,  q avanza hasta llegar a fin (f)
    //


    cout << "Hello World 123!\n";
}

#include <iostream>
using namespace std;

template <class C>
struct UTIL {
    
    
    int* split(int* ini, int* fin) {
        
        C op;
        
        int* p = ini;
        int* q = ini;
        
        while (q != fin) {
            if (op(*q)) {
                swap(*p, *q);
                p++;
            }
            q++;
        }
        return p;
    }
    
    
    void merge(int* ini, int* mid, int* fin) {
        
        int* p = ini;
        int* q = mid;
        
        while (p != q && q != fin) {
            
            if (*p <= *q) {
                ++p;
            }
            
            else {
                
                int temp = *q;
                int* r = q;
                
                while (r != p) {
                    *r = *(r - 1);
                    r++;
                }
                
                *p = temp;
                
                p++;
                q++;
                mid++;
            }
        }
    }
};


class div3 {
public:

    bool operator()(int a) {
        return !(a % 3);
    }

};


void print(int* ini, int* fin) {

    for (int* it = ini; it != fin; ++it)
        cout << *it << " ";

    cout << endl;
}


int main() {

    int arr1[] = {1,4,8,7,21,3,4,5,6,9};
    int arr2[] = {3,6,9,33,1,2,4,8,11,23,30,60};
    
    int n1 = sizeof(arr1)/sizeof(arr1[0]);
    int n2 = sizeof(arr2)/sizeof(arr2[0]);

    UTIL<div3> util;
    
    
    // Split
    print(arr1, arr1 + n1);
    util.split(arr1, arr1 + n1);
    print(arr1, arr1 + n1);
    
    cout << endl;
    
    // Merge
    print(arr2, arr2 + n2);
    util.merge(arr2, arr2 + 4, arr2 + n1);
    print(arr2, arr2 + n2);

    return 0;
}

#include <iostream>
using namespace std;

int main(){
    int a[5]={1,3,4,5,7};
    int*p=(a+2);
    int *top=(a+4);
    int *tmp=p;
    for(int i=0;i<5;i++){cout << a[i] << " ";}
    cout << endl;
    while(p<top){
        *tmp=*(tmp+1);
        tmp++;
    }  
    for(int i=0;i<5;i++){cout << a[i] << " ";}
    cout<<endl;
    tmp--;
    *tmp=10;
    for(int i=0;i<5;i++){cout << a[i] << " ";}
    return 0;
}
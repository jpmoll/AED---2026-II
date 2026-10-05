
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

using namespace std;

void expan(int x, int y);

int m[10][10] =
{
    {0,0,0,0,0,0,0,0,0,0,},
    {0,0,0,0,0,1,0,0,0,0,},
    {0,2,0,0,0,0,0,0,0,0,},
    {0,0,0,0,0,0,0,1,0,0,},
    {0,0,0,0,0,0,0,0,0,0,},
    {0,0,0,0,0,0,0,0,0,0,},
    {0,0,0,0,0,0,0,0,0,0,},
    {0,0,0,0,2,0,0,0,0,0,},
    {0,0,0,0,0,0,0,0,0,0,},
    {0,0,0,0,0,0,0,0,0,0,},
};

mutex m2[10][10];

void printm()
{
    for (int j = 0; j < 10; j++)
    {
        for (int i = 0; i < 10; i++)
        {
            if (m[j][i] == 0)
                std::cout << "  ";
            else
                std::cout << m[j][i];
        }
        std::cout << "\n";
    }
    std::cout << "---------------------------\n";
}

void levels()
{
    vector<thread> threads(4);
    threads[0] = thread(expan, 1, 5);
    threads[1] = thread(expan, 2, 1);
    threads[2] = thread(expan, 3, 7);
    threads[3] = thread(expan, 7, 4);

    for (int i = 0; i < 4; i++) {
        threads[i].join();
    }
}

void changead(int x, int y) {
    int i = m[x][y];
    bool b = 0;
    if (x > 0 && m[x - 1][y] == 0) {

        if (m2[x - 1][y].try_lock()) {
            m[x - 1][y] = i + 1;
            m2[x - 1][y].unlock();
            b = 1;
        }
    }
    if (x < 9 && m[x + 1][y] == 0) {
        if (m2[x + 1][y].try_lock()) {
            m[x + 1][y] = i + 1;
            m2[x + 1][y].unlock();
            b = 1;
        }
    }
    if (y > 0 && m[x][y - 1] == 0) {
        if (m2[x][y - 1].try_lock()) {
            m[x][y - 1] = i + 1;
            m2[x][y - 1].unlock();
            b = 1;
        }
    }
    if (y < 9 && m[x][y + 1] == 0) {
        if (m2[x][y + 1].try_lock()) {
            m[x][y + 1] = i + 1;
            m2[x][y + 1].unlock();
            b = 1;
        }
    }
    /*if (b) {
        changead(x - 1, y);
        changead(x + 1, y);
        changead(x, y - 1);
        changead(x, y + 1);
    }*/
}

void expan(int x, int y) {
    changead(x, y);
}

int main()
{
    printm();
    levels();
    printm();
}

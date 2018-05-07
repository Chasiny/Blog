#include <iostream>
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include "test.cpp"
using namespace std;

#define SIZE 10
#define TEST_SIZE 5000000

void ShuffleOriginal(int *array, int len)
{
    int newPos;
    int temp;
    bool mask[len];
    int newArray[len];
    int movePos = -1;
    memset(mask, 0, sizeof(mask));
    for (int i = len ; i > 0; i--)
    {
        newPos = (rand() % i)+1;
        int pos = 0;
        movePos=-1;
        while (pos < newPos)
        {
            movePos++;
            if (mask[movePos] == false)
            {
                pos++;
            }
        }
        mask[movePos] = true;
        newArray[i-1] = array[movePos];
    }

    for(int i=0;i<len;i++){
        array[i]=newArray[i];
    }
}

void ModernShuffle(int *array, int len)
{
    int newPos;
    int temp;
    for (int i = len - 1; i > 0; i--)
    {
        newPos = rand() % (i + 1);
        temp = array[i];
        array[i] = array[newPos];
        array[newPos] = temp;
    }
}

int main()
{
    srand(time(NULL));
    int *map = Check(ShuffleOriginal, SIZE, TEST_SIZE);
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            cout << map[i * SIZE + j] << "\t";
        }
        cout << endl;
    }
}
#include <iostream>
#include <string.h>

using namespace std;

typedef void (*ShuffleFunc)(int *array, int len);

void reset(int *array, int len)
{
    for (int i = 0; i < len; i++)
    {
        array[i] = i;
    }
}

int *Check(ShuffleFunc sf, int arraysize, int testsize)
{
    int *map = (int *)malloc(arraysize * arraysize * sizeof(int));
    memset(map, 0, sizeof(map));
    int *array = (int *)malloc(arraysize * sizeof(int));

    for (int i = 0; i < testsize; i++)
    {
        reset(array, arraysize);
        sf(array, arraysize);
        for (int i = 0; i < arraysize; i++)
        {
            map[array[i] * arraysize + i]++;
        }
    }

    return map;
}
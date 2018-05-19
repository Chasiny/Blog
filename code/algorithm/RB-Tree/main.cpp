#include <iostream>
#include "Utils.h"
#include "RedBlackTree.h"

using namespace std;

int main() {
    RedBlackTree p;

    srand(time(NULL));
    for(int i=-100;i<0;i++){
        p.Insert(rand()%50000-50000/2);
    }

    p.ToDot("tree.dot");
    return 0;
}
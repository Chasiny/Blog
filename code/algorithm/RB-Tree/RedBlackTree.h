//
// Created by chasiny on 18-5-19.
//

#ifndef RB_TREE_REDBLACKTREE_H
#define RB_TREE_REDBLACKTREE_H

#include <iostream>
#include <fstream>
#include <queue>
#include "Node.h"

using namespace std;

class RedBlackTree {
public:
    RedBlackTree();

    bool Insert(int val);
    bool Reset(Node* pos);

    void MoveLeft(Node* pos);
    void MoveRight(Node* pos);

    void ToDot(string file);

    int count;
    Node* head;
    Node* nil;
};


#endif //RB_TREE_REDBLACKTREE_H

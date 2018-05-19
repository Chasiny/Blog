//
// Created by chasiny on 18-5-19.
//

#ifndef RB_TREE_NODE_H
#define RB_TREE_NODE_H

#include <iostream>
using namespace std;

class Node {
public:
    Node();
    Node(int id,Node* parent,int val,bool color);

    int id;
    Node* parent;
    int val;
    bool color;
    Node* left;
    Node* right;
};


#endif //RB_TREE_NODE_H

//
// Created by chasiny on 18-5-19.
//

#include "Node.h"

Node::Node() {
    id=-1;
    val=-1;
    color=true;
    left=right=NULL;
}

Node::Node(int id,Node* parent,int val,bool color){
    this->id=id;
    this->parent=parent;
    this->color=color;
    this->val=val;
    left=right=NULL;
}
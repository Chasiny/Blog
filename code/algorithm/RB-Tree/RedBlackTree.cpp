//
// Created by chasiny on 18-5-19.
//

#include "RedBlackTree.h"
#include "Utils.h"

RedBlackTree::RedBlackTree() {
    nil=new Node(-1,NULL,-1,false);
    nil->color=false;

    count=0;
    head=new Node(count,NULL,0,true);
    count++;
    head->left=head->right=nil;
}

bool RedBlackTree::Insert(int val) {
    Node *newNode = new Node(this->count,NULL,val,true);
    newNode->left=newNode->right=this->nil;
    this->count++;

    if(head->left==nil){
        head->left=newNode;
        newNode->color=false;
        newNode->parent=head;
        return true;
    }

    Node* pos=this->head->left;
    while(true){
        if(val>pos->val){
            if(pos->right==this->nil){
                pos->right=newNode;
                newNode->parent=pos;
                break;
            }
            pos=pos->right;
        }else if(val<pos->val){
            if(pos->left==this->nil){
                newNode->parent=pos;
                pos->left=newNode;
                break;
            }
            pos=pos->left;
        }else{
            cout<<"no add\n";
            delete(newNode);
            return false;
        }
    }

    this->Reset(newNode);
}


bool RedBlackTree::Reset(Node* pos){
    while(pos->parent!=NULL&&pos->parent->color&&pos->color){
        cout<<"reset :"<<pos->val<<endl;
        if(pos->parent!=NULL&&pos->parent->parent!=NULL){
            Node* uncle=pos->parent==pos->parent->parent->left?pos->parent->parent->right:pos->parent->parent->left;
            if(pos->parent->color&&uncle!=NULL&&uncle->color){
                uncle->color=pos->parent->color=false;
                pos->parent->parent->color=true;
                pos=pos->parent->parent;
            }else if(uncle!=NULL&&!uncle->color){
                if(pos->parent==pos->parent->parent->left){
                    if(pos==pos->parent->right){
                        this->MoveLeft(pos->parent);
                        pos=pos->left;
                    }else{
                        pos->parent->color=false;
                        pos->parent->parent->color=true;
                        this->MoveRight(pos->parent->parent);
                    }
                }else{
                    if(pos==pos->parent->left){
                        this->MoveRight(pos->parent);
                        pos=pos->right;
                    }else{
                        pos->parent->color=false;
                        pos->parent->parent->color=true;
                        this->MoveLeft(pos->parent->parent);
                    }
                }
            }
        }

        this->head->left->color=false;
    }
}


void RedBlackTree::MoveLeft(Node* pos) {
    if(pos->right==NULL||pos->parent==NULL){
        return;
    }

    Node*& p=pos->parent->left==pos?pos->parent->left:pos->parent->right;

    p=pos->right;
    pos->right->parent=pos->parent;
    pos->parent=pos->right;
    pos->right=pos->parent->left;
    pos->parent->left=pos;

    if(pos->right!=NULL){
        pos->right->parent=pos;
    }
}

void RedBlackTree::MoveRight(Node* pos) {
    if(pos->left==NULL||pos->parent==NULL){
        return;
    }

    Node*& p=pos->parent->left==pos?pos->parent->left:pos->parent->right;
    p=pos->left;
    pos->left->parent=pos->parent;
    pos->parent=pos->left;
    pos->left=pos->parent->right;
    pos->parent->right=pos;

    if(pos->left!=NULL){
        pos->left->parent=pos;
    }
}

void RedBlackTree::ToDot(string file) {
    string ans = "digraph abc{node [shape = circle];" +
            Utils::CreateNode(this->head->id, this->head->val, this->head->color ? "red" : "grey");
    queue<Node *> nodeQueue;
    nodeQueue.push(this->head);

    while (!nodeQueue.empty()) {
        Node *pos = nodeQueue.front();
        nodeQueue.pop();

        if (pos->left != NULL&&pos->left!=this->nil) {
            nodeQueue.push(pos->left);
            ans = ans + Utils::CreateNode(pos->left->id, pos->left->val, pos->left->color ? "red" : "grey");
            ans = ans + Utils::CreateLine(pos->id, pos->left->id);
        }
        if (pos->right != NULL&&pos->right!=this->nil) {
            nodeQueue.push(pos->right);
            ans = ans + Utils::CreateNode(pos->right->id, pos->right->val, pos->right->color ? "red" : "grey");
            ans = ans + Utils::CreateLine(pos->id, pos->right->id);
        }
    }

    ans = ans + "}";

    ofstream out;
    out.open(file, ios::out | ios::trunc);
    out << ans;

    return;
}
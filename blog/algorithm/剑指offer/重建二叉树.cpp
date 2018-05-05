
// 重建二叉树
// 输入某二叉树的前序遍历和中序遍历的结果，请重建出该二叉树。
// 假设输入的前序遍历和中序遍历的结果中都不含重复的数字。
// 例如输入前序遍历序列{1,2,4,7,3,5,6,8}和中序遍历序列{4,7,2,1,5,3,8,6}，
// 则重建二叉树并返回。
#include <iostream>
#include <cstdio>
#include <stdlib.h> 
#include <time.h>
#include <vector>

using namespace std; 

struct TreeNode {
    int val;
    TreeNode *left;
    TreeNode *right;
};

TreeNode* getChildTree(vector<int> pre,int pl,int pr,vector<int> vin,int vl,int vr){
    if(pl>pr){
        return NULL;
    }

    TreeNode* ans=new TreeNode;
    ans->val=pre[pl];

    if(pl==pr){
        ans->left=ans->right=NULL;
        return ans;
    }

    int vinPos=vl;
    for(;vinPos<=vr;vinPos++){
        if(vin[vinPos]==pre[pl]){
            break;
        }
    }

    ans->left=getChildTree(pre,pl+1,pl+(vinPos-vl),vin,vl,vinPos-1);
    ans->right=getChildTree(pre,pl+(vinPos-vl)+1,pr,vin,vinPos+1,vr);

    return ans;

}

TreeNode* reConstructBinaryTree(vector<int> pre,vector<int> vin) {
    return getChildTree(pre,0,pre.size()-1,vin,0,vin.size()-1);
}

void PreShow(TreeNode* s){
    if(s==NULL){
        return;
    }
    printf("%d ",s->val);
    PreShow(s->left);
    PreShow(s->right);
}

void MidShow(TreeNode* s){
    if(s==NULL){
        return;
    }
    MidShow(s->left);
    printf("%d ",s->val);
    MidShow(s->right);
}

void BackShow(TreeNode* s){
    if(s==NULL){
        return;
    }
    MidShow(s->left);
    MidShow(s->right);
    printf("%d ",s->val);
}


int main(){
    int pre[]={1,2,3,4,5,6,7};
    int vin[]={3,2,4,1,6,5,7};
    vector<int> preVector(pre,pre+sizeof(pre)/sizeof(int));
    vector<int> vinVector(vin,vin+sizeof(vin)/sizeof(int));

    TreeNode* tn=reConstructBinaryTree(preVector,vinVector);

    PreShow(tn);
    printf("\n");
    MidShow(tn);
    printf("\n");
    BackShow(tn);
    printf("\n");


    return 0;
}
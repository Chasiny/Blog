
#include <cstdio>
struct ListNode {
    int val;
    ListNode *next;
};

class Solution {
public:
    ListNode* FindKthToTail(ListNode* pListHead, unsigned int k) {
        if(k==0)return NULL;
        ListNode* kPos=NULL;
        ListNode* listPos=pListHead;
        int pos=1;
        while(listPos){
            if(pos==k){
                kPos=pListHead;
            }else if(pos>k){
                kPos=kPos->next;
            }
            listPos=listPos->next;
            pos++;
        }
        
        return kPos;
    }
};
#include <iostream>
#include <cstdio>
#include <stdlib.h> 
#include <time.h>
#include <stack>

using namespace std;

class Solution
{
public:
    void push(int node) {
        stack1.push(node);
    }

    int pop() {
        if(stack1.size()<=0){
            return -1;
        }

        while(stack1.size()>0){
            stack2.push(stack1.top());
            stack1.pop();
        }

        int ans=stack2.top();
        stack2.pop();
        while(stack2.size()>0){
            stack1.push(stack2.top());
            stack2.pop();
        }

        return ans;
    }

private:
    stack<int> stack1;
    stack<int> stack2;
};

int main(){
    Solution s;
    s.push(1);
    s.push(2);
    s.push(3);
    s.push(4);

    for(int i=0;i<4;i++){
        printf("%d \n",s.pop());
    }
}
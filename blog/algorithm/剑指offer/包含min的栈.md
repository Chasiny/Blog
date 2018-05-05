```c++
class Solution {
public:
    int table[1000];
    int pos=-1;
    void push(int value) {
        pos++;
        table[pos]=value;
    }
    void pop() {
        pos--;
    }
    int top() {
        return table[pos];
    }
    int min() {
        int min=table[0];
        for(int i=1;i<=pos;i++){
            if(min>table[i]){
                min=table[i];
            }
        }
        return min;
    }
};
```
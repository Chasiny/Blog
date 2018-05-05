class Solution {
public:
    int table[1000];
    Solution(){
        table[0]=0;
        table[1]=1;
        table[2]=2;
        for(int i=3;i<1000;i++){
            table[i]=table[i-1]+table[i-2];
        }
    }
    int rectCover(int number) {
        return table[number];
    }
};
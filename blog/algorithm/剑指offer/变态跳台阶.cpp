class Solution {
public:
    int table[1000];
    Solution(){
        table[0]=0;
        table[1]=1;
        table[2]=2;
        for(int i=3;i<1000;i++){
            table[i]=0;
            for(int j=1;j<i;j++){
                table[i]+=table[j];
            }
            table[i]++;
        }
    }
    int jumpFloorII(int number) {
        return table[number];
    }
};
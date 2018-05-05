class Solution {
public:
     int  NumberOf1(int n) {
         int count=0;
         for(int i=0;i<32;i++){
             if(n&1==1){
                 count++;
             }
             n=n>>1;
         }
         return count;
     }
};
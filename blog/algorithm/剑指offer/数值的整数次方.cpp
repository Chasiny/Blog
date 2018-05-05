class Solution {
public:
    double Power(double base, int exponent) {
        if(exponent==0){
            return 1;
        }
        if(exponent==1){
            return base;
        }
        if(exponent==-1){
            return 1/base;
        }
        
        double ans=Power(base,exponent/2);
        if((exponent>0?exponent:-exponent)%2==0){
            return ans*ans;
        }
        if(exponent>0){
            return ans*ans*base;
        }
        return ans*ans*(1/base);
    }
    
};
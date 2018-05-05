#include <iostream>
#include <cstdio>
#include <stdlib.h> 
#include <time.h>
#include <vector>
using namespace std; 

class Solution {
public:
    int minNumberInRotateArray(vector<int> rotateArray) {
        if(rotateArray.size()<=0){
            return 0;
        }
        int min=9999999;
        for(int i=0;i<rotateArray.size();i++){
            if(min>rotateArray[i]){
                min=rotateArray[i];
            }
        }
        
        return min;
    }
};
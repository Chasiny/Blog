//
// Created by chasiny on 18-5-18.
//

#include "Utils.h"

string Utils::IntToString(int a) {
    if(a==0){
        return "0";
    }
    bool flag=false;
    if(a<0){
        flag=true;
        a=-a;
    }
    string ans="";
    while(a>0){
        ans=char(a%10+'0')+ans;
        a=a/10;
    }
    return flag?"-"+ans:ans;
}

string Utils::CreateNode(int id, int val, string color) {
    return IntToString(id)+"[style=\"filled\",label=\""+IntToString(val)+"\", fillcolor=\""+color+"\"];";
}

string Utils::CreateLine(int p, int c) {
    return IntToString(p)+"->"+IntToString(c)+";";
}
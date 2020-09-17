#include<map>
#include<algorithm>
#include<vector>
#include<iostream>
int main(){
    std::vector<int> wedc{1,2,3};
    std::cout<<*wedc.begin();
    std::sort(wedc.begin(),wedc.end());
}
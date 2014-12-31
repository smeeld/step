#include <iostream>
struct A {
 int a;
};
int main(){
 A a;
 A& b=a;

 a.a=23;
 --b.a;
std::cout<<"Ok "<<a.a<<"\n";
};

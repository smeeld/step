#include <stdio.h>
#include <stdlib.h>
int* m;
int main(int argc, char* s[]){
int i;
int num=atoi(s[1]);
int tmp;
int c;
int j;
m=malloc(sizeof(int)*num);
for(i=0;i<num;i++){
 m[i]=random();
 };
j=2;
i=1;
while(i<num)
{

if(m[i-1]>m[i]){ tmp=m[i]; m[i]=m[i-1];m[i-1]=tmp; i--; if(i==0){i=j; j++; }; }else{i=j; j++; };
 };
for(i=0;i<100;i++)
 {
  printf("NUM==%d\n",m[i]);
  };
  free(m);
}

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
int* m;
 int cmp(const void* x1,const void* x2){
  return (*(int*)x1>*(int*)x2);
   };
int main(int c,char* s[]){
int i;
int out=atoi(s[2]);
long num=atol(s[1]);
m=malloc(sizeof(int)*num);
for(i=0;i<num;i++){
 m[i]=random();
 };
 qsort(m, num, sizeof(int), cmp);

for(i=0;i<out;i++)
 {
  printf("NUM==%d\n",m[i]);
  };

 free(m);
 };

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int cmp(void* a, void* b){ return *(int*)a < *(int*)b; };
typedef unsigned in;
  void srt(int* st, int* lst, int (*cmp)(void*, void*)){

    int *tmp=st, *md=st, *sp;
   int n=0;
  int mid=*st;
   while(tmp==st){ if(md==lst){ return; };
           sp=lst;
          
         while(sp!=tmp){
          n=0;
     while(cmp(tmp, md)){ ++tmp; if(tmp==sp){ n=1; break; }; };
           if(n){ break; };
     do{ --sp; if(tmp==sp){ n=1; break; }; }while(cmp(md, sp));
              if(n){ break; };
           n=*tmp; *tmp=*sp; *sp=n; 
     ++tmp;
    };  ++md; mid=*md;
  };
  
   srt(st, tmp, cmp);
   srt(tmp, lst, cmp);
};
 
 int main(int arg, char * s[]){
  int i;
  int* m;
  in num=atoi(s[1]);
  in out=atoi(s[2]);

   m=malloc(sizeof(int)*num);
  
 for(i=0;i<num;i++)
  {
   m[i]=random();
    };

  srt(m, m+num-1, cmp);



for(i=0;i<out;i++)
 {
  printf("NUm==%d\n",m[i]);
  };

 free(m);
  };

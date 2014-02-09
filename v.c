#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(int arg,char* s[]){

int c;
char * m="5553424312345678000000000000061e000000000000000000000000000000";
char message2[strlen(m)];
 strncpy(message2,m,strlen(m));

printf("OK %x\n",message2[2]);
c=atoi(s[1]);char f='55';
printf("CHAR==%x INT==%c    XX=%x GG=%c HH=%c\n",c,s[1][0],s[1][0],f,f);

}

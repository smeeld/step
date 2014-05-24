
#include <models.h>

int sphere::create(model* p) const { 
 
 int sz=p->texcoord_number;
 int Y=sqrt(sz);
 
int x, y;
float a=0, z=0;
float* s=p->vertex; GLuint* i=p->index;
float X=(float)Y;
float sq2=2*M_PI/(X-1);
float sq=M_PI/(X-1);
for(x=0;x<Y;x+=1){
    z=0;
  for(y=0;y<Y;y+=1){
 s[3*Y*x+3*y]=1.0*sin(a)*sin(z);
 s[3*Y*x+3*y+1]=1.0*cos(a);
 s[3*Y*x+3*y+2]=1.0*sin(a)*cos(z);
 s[3*sz+3*Y*x+3*y]=sin(a)*sin(z);
 s[3*sz+3*Y*x+3*y+1]=sin(a)*cos(z);
 s[3*sz+3*Y*x+3*y+2]=cos(a);
 s[6*sz+2*Y*x+2*y]=x/X;
 s[6*sz+2*Y*x+2*y+1]=y/X;
   z+=sq2; };
 a+=sq;};

for(x=0;x<Y-1;x+=1){
for(y=0;y<Y-1;y++){
i[6*Y*x+6*y]=x*Y+y;
i[6*Y*x+6*y+1]=x*Y+Y+y;
i[6*Y*x+6*y+2]=x*Y+y+1;
i[6*Y*x+6*y+3]=x*Y+y+1;
i[6*Y*x+6*y+4]=x*Y+y+Y;
i[6*Y*x+6*y+5]=x*Y+Y+y+1;
  };
 };
 
  return 0;
   };


#version 130
attribute vec2 texcoord;
attribute vec3 vec;
uniform int i;
out vec2 texc;
void main(){
  if(i==1){gl_Position=gl_ModelViewProjectionMatrix*vec4(vec,1.0); texc=texcoord;}
  if(i==0){gl_Position=gl_ModelViewProjectionMatrix*gl_Vertex; texc=gl_MultiTexCoord0.st;}
}

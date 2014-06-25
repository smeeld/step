#version 130
attribute vec2 texcoord;
attribute vec3 vec;
out vec2 mid;
void main(){
  gl_Position=gl_ModelViewProjectionMatrix*vec4(vec,1.0)*vec4(sin(vec.x), vec.y, cos(vec.z), 1.0);
   mid=texcoord;
  }

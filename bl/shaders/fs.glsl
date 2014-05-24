#version 130
uniform sampler2D texa;
uniform sampler2D texb;
uniform sampler2D texc;
in vec2 mid;
void main(){
 vec2 y1=vec2(mid.x, mid.y);
 vec4 v1=texture2D(texa, y1);
 vec4 v2=texture2D(texb, y1);
 gl_FragColor=v1+v2;
}

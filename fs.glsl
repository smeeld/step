#version 130
uniform sampler2D tex;
uniform sampler2D tex2;
in vec2 texc;
void main(){
 vec2 y1=vec2(texc.x, texc.y);
 vec2 y2=vec2(texc.x, texc.y*0.5+0.5);
 vec4 v1=texture2D(tex2, y1);
 vec4 v2=texture2D(tex2,y2);
 gl_FragColor=v1;
}

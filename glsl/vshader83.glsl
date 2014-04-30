#version 150 

out vec4 color;
in  vec3 vPosition;
uniform vec4 vNormal;
uniform vec4 vColor;
uniform mat4 modelview;
uniform mat4 projection;


void main() 
{
    gl_PointSize = 1.2;//10*vPosition.z;
    gl_Position =modelview* vec4(vPosition.y, vPosition.x,0,1);
    color = vec4(1,0,0,1);
} 

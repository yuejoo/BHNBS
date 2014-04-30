#version 150 

attribute vec4 vPosition;
attribute vec3 vShift;

out vec4 color;

uniform vec4 vColor;
uniform mat4 modelview;
uniform mat4 camview;
uniform mat4 projection;

vec4 tPosition;
void main() 
{
    color = vColor;
    
    tPosition = modelview*vPosition;
    
    tPosition.x += vShift.x;
    tPosition.y += vShift.y;
    tPosition.z *= vShift.z;
    gl_Position = projection*camview*tPosition;
} 

#version 460 core


uniform mat4 modelingMatrix;
uniform mat4 viewingMatrix;
uniform mat4 projectionMatrix;
uniform vec3 eyePos;

layout(location=0) in vec3 inVertex;
layout(location = 1) in vec2 aTexCoord;
out vec2 texCoord;
void main(void)
{
    
    gl_Position = projectionMatrix * viewingMatrix * modelingMatrix * vec4(inVertex, 1);
    texCoord = aTexCoord;
}


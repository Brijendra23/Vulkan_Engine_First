#version 450

layout(location=0) in vec3 pos;
layout(location=1) in vec3 col;

layout(location=0) out vec3 fragCol;
void main()
{
//built in values such as gl_positon refers to position of the point 
//built in such as gl_vertexIndex refers to the index of the vector that we are going to use
gl_Position=vec4(pos,1.0);

fragCol=col;
}
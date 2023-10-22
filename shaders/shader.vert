#version 450

layout(location=0) out vec3 fragColor;//output color for the vertex (location is required)

//triangle vertex
vec3 positions[3]=vec3[](
vec3(0.0,-0.4,0.0),
vec3(0.4,0.4,0.0),
vec3(-0.4,0.4,0.0)
);


//color for the triangle
vec3 colors[3]=vec3[]
(vec3(1.0,0.0,0.0),
vec3(0.0,1.0,0.0),
vec3(0.0,0.0,1.0)
);

void main()
{
//built in values such as gl_positon refers to position of the point 
//built in such as gl_vertexIndex refers to the index of the vector that we are going to use
gl_Position=vec4(positions[gl_VertexIndex],1.0);

fragColor=colors[gl_VertexIndex];

}
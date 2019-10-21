#version 400            
uniform mat4 M;
uniform mat4 P;
uniform mat4 V;

layout (location = 0) in vec3 pos_attrib;
layout (location = 1) in vec3 aNormal;

out vec3 FragPos;
out vec3 Normal;

void main(void)
{
	FragPos = vec3(M * vec4(pos_attrib, 1.0));
    Normal = aNormal;  
	gl_Position = P * V * vec4(FragPos, 1.0);
}
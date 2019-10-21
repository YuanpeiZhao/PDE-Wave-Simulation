#version 400            
uniform mat4 PVM;
uniform mat4 P;
uniform mat4 VM;

uniform sampler2D tex;

uniform float time;
uniform int cur_frame;
uniform float d;

layout (location = 0) in vec3 pos_attrib;
layout (location = 1) in vec2 tex_coord_attrib;

out vec3 Normal;  
out vec3 FragPos; 

void main(void)
{
	vec4 pixel = texture(tex, tex_coord_attrib);

	float z_pre1_l = texture2D(tex, tex_coord_attrib - vec2(d, 0.0f))[(cur_frame+2)%3];
	float z_pre1_r = texture2D(tex, tex_coord_attrib + vec2(d, 0.0f))[(cur_frame+2)%3];
	float z_pre1_u = texture2D(tex, tex_coord_attrib - vec2(0.0f, d))[(cur_frame+2)%3];
	float z_pre1_d = texture2D(tex, tex_coord_attrib + vec2(0.0f, d))[(cur_frame+2)%3];

	gl_Position = P*VM*vec4(pos_attrib + vec3(0.0f, 0.0f, pixel[cur_frame]), 1.0);

	FragPos = gl_Position.xyz;
	Normal = vec3(z_pre1_l - z_pre1_r, z_pre1_d - z_pre1_u, 2.0f * d);
}
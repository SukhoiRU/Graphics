#version 330 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec3 texcoords;
out vec2 TexCoords;

void main()
{
	gl_Position	= vec4(position.xy, 0.0, 1.0);
	TexCoords	= texcoords.xy;
}

#version 330 core

layout(location = 0) in vec4 position;
out vec2 TexCoords;

void main()
{
	gl_Position	= vec4(position.xy, 0.0, 1.0);
	TexCoords	= position.zw;
}

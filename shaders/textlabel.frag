#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D text;
uniform vec3 textColor;
uniform float alpha;

void main()
{
	vec4 sampled	= vec4(1.0, 1.0, 1.0, texture(text, TexCoords).a);
	//if(sampled.a < 0.1)	sampled.a = 0.1;
	color	= vec4(textColor, alpha)*sampled;
}

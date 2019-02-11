#version 330 core

in vec2 TexCoords;
out vec4 fColor;
uniform sampler2D text;

void main()
{
   fColor = texture(text, TexCoords);
}

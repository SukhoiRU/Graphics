#version 330 core

in vec4 gColor;
out vec4 fColor;

void main()
{
   fColor = gColor;
//   fColor = vec4(0.,0.,1.,1.);
}
  
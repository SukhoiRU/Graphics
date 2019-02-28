#version 330 core

flat in vec4 vColor;
in float L;
out vec4 fColor;

uniform float tick;
uniform float toc;
uniform float dL;

void main()
{
   //Считаем расстояние до точки
   float l  = L + dL - int((L + dL)/(tick+toc))*(tick+toc);
   if(abs(l) <= tick)  fColor = vColor;
   else discard;
}
  
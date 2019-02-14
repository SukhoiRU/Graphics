#version 330 core

layout(location = 0) in vec2 vertex;
layout(location = 1) in vec3 text;
out vec3 TexCoords;

uniform mat4 modelToWorld;
uniform mat4 worldToCamera;
uniform mat4 cameraToView;

void main()
{
  vec4 pos		= worldToCamera * modelToWorld * vec4(vertex.xy, 0.0, 1.0);
  pos.x			= int(pos.x);
  pos.y			= int(pos.y);
  gl_Position	= cameraToView * pos;
  TexCoords		= text;
}

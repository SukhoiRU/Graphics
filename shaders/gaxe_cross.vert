#version 330 core

layout(location = 0) in vec2 vertex;
layout(location = 1) in vec2 tex;
out vec2 TexCoords;

uniform mat4 modelToWorld;
uniform mat4 worldToCamera;
uniform mat4 cameraToView;

void main()
{
  gl_Position	= cameraToView * worldToCamera * modelToWorld * vec4(vertex.xy, 0.0, 1.0);
  TexCoords		= tex;
}

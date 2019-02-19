#version 330 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec3 color;
flat out vec4 vColor;

uniform mat4 modelToWorld;
uniform mat4 worldToCamera;
uniform mat4 cameraToView;

void main()
{
  gl_Position = cameraToView * worldToCamera * modelToWorld * vec4(position, 0.0, 1.0);
  vColor = vec4(color, 1.0);
}

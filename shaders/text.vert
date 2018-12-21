#version 330 core

layout(location = 0) in vec4 vertex;
out vec2 TexCoords;

uniform mat4 modelToWorld;
uniform mat4 worldToCamera;
uniform mat4 cameraToView;

void main()
{
  vec4 pos = worldToCamera * modelToWorld * vec4(vertex.xy, 0.0, 1.0);
  pos.x       = int(pos.x);
  pos.y       = int(pos.y);
  gl_Position = cameraToView * pos;

  //gl_Position = cameraToView * worldToCamera * modelToWorld * vec4(vertex.xy, 0.0, 1.0);

  TexCoords	= vertex.zw;
}

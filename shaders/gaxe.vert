#version 330

layout(location = 0) in vec2 position;
out vec4 vColor;

uniform vec3 color;
uniform mat4 modelToWorld;
uniform mat4 worldToCamera;
uniform mat4 cameraToView;

void main()
{
    vec4 pos = worldToCamera * modelToWorld * vec4(position, 0.0, 1.0);
    pos.x       = int(pos.x) + 0.5;
    pos.y       = int(pos.y) + 0.5;
    gl_Position = cameraToView * pos;

  //gl_Position = cameraToView * worldToCamera * modelToWorld * vec4(position, 0.0, 1.0);
  vColor = vec4(color, 1.0);
}

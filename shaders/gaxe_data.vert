#version 330 core

layout(location = 0) in vec2 position;
out vec4 vColor;
out vec4 vZeroPoint;

uniform mat4 modelToWorld;
uniform mat4 worldToCamera;
uniform mat4 cameraToView;
uniform vec3 color;
uniform float alpha;
uniform int round;

void main()
{
    vec4 pos = worldToCamera * modelToWorld * vec4(position, 0.0, 1.0);
    vec4 zero = worldToCamera * modelToWorld * vec4(position.x, 0.0, 0.0, 1.0);
	if(round != 0)
    {
		pos.x       = int(pos.x) + 0.5;
		pos.y       = int(pos.y) + 0.5;
		zero.x       = int(zero.x) + 0.5;
		zero.y       = int(zero.y) + 0.5;
	}
	gl_Position = cameraToView * pos;
	vZeroPoint = cameraToView * zero;
	vColor = vec4(color, alpha);
}

#version 330 core

layout(location = 0) in vec2 position;
out vec4 vColor;

uniform mat4 modelToWorld;
uniform mat4 worldToCamera;
uniform mat4 cameraToView;
uniform vec3 color;
uniform float alpha;
uniform int round;
uniform int lineType;
uniform float baseLine;

void main()
{
	if(lineType == 3)
	{
		//Отрисовка bool
		vec4 pos = worldToCamera * modelToWorld * vec4(position.x, 0.0, 0.0, 1.0);
		if(round != 0)
		{
			pos.x       = int(pos.x) + 0.5;
			pos.y       = int(pos.y) + 0.5;
		}
		gl_Position = cameraToView * pos;

		//В сигнале Z передаем значение bool
		gl_Position.z = position.y;
	}
	else
	{
		vec4 pos = worldToCamera * modelToWorld * vec4(position, 0.0, 1.0);
		if(round != 0)
		{
			pos.x       = int(pos.x) + 0.5;
			pos.y       = int(pos.y) + 0.5;
		}
		gl_Position = cameraToView * pos;
	}
    vColor = vec4(color, alpha);
}

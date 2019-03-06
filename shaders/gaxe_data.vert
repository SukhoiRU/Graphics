#version 330 core

layout(location = 0) in vec2 position;
flat out vec4 vColor;

uniform mat4 modelToWorld;
uniform vec3 color;
uniform float alpha;
uniform int lineType;

void main()
{
	//Выдаем дальше в миллиметрах документа
	gl_Position = modelToWorld * vec4(position, 0.0, 1.0);
	if(lineType == 3)
	{
		//В сигнале Z передаем значение bool
		gl_Position.z = position.y;
	}
    vColor = vec4(color, alpha);
}

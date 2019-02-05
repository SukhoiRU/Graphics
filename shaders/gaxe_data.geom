#version 330 core
layout (lines) in;
layout (line_strip, max_vertices = 6) out;

in vec4 vColor[];
out vec4 gColor;

uniform int lineType;
uniform float baseLine;
uniform vec2 pixelSize;

void main() 
{ 
	gColor = vColor[0];

	switch(lineType)
	{
		case 1:
		{
			//Ступеньки
			gl_Position = gl_in[0].gl_Position; 
			EmitVertex();
			gl_Position.x = gl_in[1].gl_Position.x; 
			gl_Position.y = gl_in[0].gl_Position.y; 
			EmitVertex();
			EndPrimitive();

			gl_Position.x = gl_in[1].gl_Position.x; 
			gl_Position.y = gl_in[0].gl_Position.y; 
			EmitVertex();
			gl_Position = gl_in[1].gl_Position; 
			EmitVertex();
			EndPrimitive();
		}break;

		case 2:
		{
			//Вертикальные палки от baseLine
			//if(gl_in[1].gl_Position.x - gl_in[0].gl_Position.x > pixelSize.x ||
			//   gl_in[1].gl_Position.y - gl_in[0].gl_Position.y > pixelSize.y ||
			//   gl_in[1].gl_Position.y - gl_in[0].gl_Position.y < -pixelSize.y)
			{
				gl_Position 	= gl_in[0].gl_Position; 
				gColor.a		= 0.2;
				EmitVertex();
				gl_Position 	= vec4(gl_in[0].gl_Position.x, baseLine, 0., 1.);
				gColor.a		= 0.2;
				EmitVertex();
				EndPrimitive();
			}
		}break;

		case 3:
		{
			///////////////////////////////////////////////////////////////////////////////
			//Отрисовка сигналов bool

			//Средняя линия
			gl_Position 	= vec4(gl_in[0].gl_Position.xy, 0.0, 1.0); 
			EmitVertex();
			gl_Position 	= vec4(gl_in[1].gl_Position.xy, 0.0, 1.0); 
			EmitVertex();
			EndPrimitive();

			if(gl_in[0].gl_Position.z != 0)
			{
				//Считаем размер пикселя в NDC
				vec4 dy	= vec4(0.0, pixelSize.y, 0.0, 0.0);

				//Верхняя линия
				gl_Position 	= vec4(gl_in[0].gl_Position.xy, 0.0, 1.0) + dy; 
				EmitVertex();
				gl_Position 	= vec4(gl_in[1].gl_Position.xy, 0.0, 1.0) + dy; 
				EmitVertex();
				EndPrimitive();

				//Нижняя линия
				gl_Position 	= vec4(gl_in[0].gl_Position.xy, 0.0, 1.0) - dy; 
				EmitVertex();
				gl_Position 	= vec4(gl_in[1].gl_Position.xy, 0.0, 1.0) - dy; 
				EmitVertex();
				EndPrimitive();
			}
		}break;

	default:
		{
			//Обычная линия
			gl_Position = gl_in[0].gl_Position; 
			EmitVertex();
			gl_Position = gl_in[1].gl_Position; 
			EmitVertex();
			EndPrimitive();
		}break;
	}
}  

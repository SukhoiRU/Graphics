#version 330 core
layout (lines) in;
layout (line_strip, max_vertices = 6) out;

in vec4 vColor[];
in vec4 vZeroPoint[];
out vec4 gColor;

uniform int lineType;

void main() { 

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
			//Вертикальные палки от нуля
			gl_Position 	= gl_in[0].gl_Position; 
			EmitVertex();
			gl_Position 	= vZeroPoint[0]; 
			if(gl_Position.y < -1.0)	gl_Position.y = -1.0;
			EmitVertex();
			EndPrimitive();

			gl_Position 	= vZeroPoint[0]; 
			if(gl_Position.y < -1.0)	gl_Position.y = -1.0;
			EmitVertex();
			gl_Position 	= vZeroPoint[1]; 
			if(gl_Position.y < -1.0)	gl_Position.y = -1.0;
			EmitVertex();
			EndPrimitive();
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

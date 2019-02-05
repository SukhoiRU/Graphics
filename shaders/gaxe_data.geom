#version 330 core
layout (lines) in;
layout (line_strip, max_vertices = 4) out;

in vec4 vColor[];
in vec4 vZeroPoint[];
out vec4 gColor;

uniform int lineType;

void main() { 

	gColor = vColor[0];

	if(lineType == 0)
	{
		//Обычная линия
		gl_Position = gl_in[0].gl_Position; 
		EmitVertex();
		gl_Position = gl_in[1].gl_Position; 
		EmitVertex();
	    EndPrimitive();
	}   
	else
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
	}

}  

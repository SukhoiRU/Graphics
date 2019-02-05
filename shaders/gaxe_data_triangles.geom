#version 330 core
layout (lines) in;
layout (triangle_strip, max_vertices = 6) out;

in vec4 vColor[];
in vec4 vZeroPoint[];
out vec4 gColor;

uniform int lineType;

void main() { 

	gColor = vColor[0];

	//Заливка
	gl_Position 	= vZeroPoint[0]; 
	EmitVertex();
	gl_Position 	= gl_in[0].gl_Position; 
	EmitVertex();
	gl_Position 	= gl_in[1].gl_Position; 
	EmitVertex();
	EndPrimitive();

	gl_Position 	= vZeroPoint[0]; 
	EmitVertex();
	gl_Position 	= gl_in[1].gl_Position; 
	EmitVertex();
	gl_Position 	= vZeroPoint[1]; 
	EmitVertex();
	EndPrimitive();
}  

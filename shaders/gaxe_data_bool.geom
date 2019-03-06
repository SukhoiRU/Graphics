#version 330 core
layout (lines) in;
layout (line_strip, max_vertices = 2) out;

uniform mat4 worldToCamera;
uniform mat4 cameraToView;
uniform int round;

uniform int lineType;
uniform float baseLine;
uniform vec2 pixelSize;

uniform float linewidth, antialias;

flat in vec4 vColor[];

out GS_OUT
{
	vec2 coord;			//Координаты углов в мм относительно линии
	flat vec4 	gColor;
	flat float	L;		//Длина отрезка
	flat float	bValue;
}gs_out;

void main() 
{ 
	gs_out.gColor	= vColor[0];
	gs_out.bValue	= gl_in[0].gl_Position.z;

		///////////////////////////////////////////////////////////////////////////////
		//Отрисовка сигналов bool
		float	len		= gl_in[1].gl_Position.x - gl_in[0].gl_Position.x;

		gs_out.bValue	= gl_in[0].gl_Position.z;
		gs_out.L		= len;

		//Получаем масштаб
		float	pix		= 1.5/worldToCamera[0][0];
		vec4	base	= worldToCamera * gl_in[0].gl_Position;
		base.y			= int(base.y) + 0.5;
		base			= inverse(worldToCamera)*base;

		gs_out.coord	= vec2(0, 1.0);
		gl_Position 	= cameraToView * worldToCamera * (base + vec4(0.,pix, 0., 0.)); 
		EmitVertex();

		gs_out.coord	= vec2(0, -1.0);
		gl_Position 	= cameraToView * worldToCamera * (base + vec4(0.,-pix, 0., 0.)); 
		EmitVertex();

		gs_out.coord	= vec2(len, 1.0);
		gl_Position 	= cameraToView * worldToCamera * vec4(gl_in[1].gl_Position.x, base.y + pix, 0., 1.); 
		EmitVertex();

		gs_out.coord	= vec2(len, -1.0);
		gl_Position 	= cameraToView * worldToCamera * vec4(gl_in[1].gl_Position.x, base.y - pix, 0., 1.); 
		EmitVertex();
		EndPrimitive();
}  

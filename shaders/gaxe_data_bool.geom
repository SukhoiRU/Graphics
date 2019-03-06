#version 330 core
layout (lines) in;
layout (line_strip, max_vertices = 6) out;

uniform mat4 worldToCamera;
uniform mat4 cameraToView;

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
	///////////////////////////////////////////////////////////////////////////////
	//Отрисовка сигналов bool
	gs_out.gColor	= vColor[0];
	gs_out.bValue	= gl_in[0].gl_Position.z;
	gs_out.L		= 0.;
	gs_out.coord	= vec2(0.);

	//Средняя линия
	gl_Position 	= cameraToView * worldToCamera * vec4(gl_in[0].gl_Position.xy, 0.0, 1.0); 
	EmitVertex();
	gl_Position 	= cameraToView * worldToCamera * vec4(gl_in[1].gl_Position.xy, 0.0, 1.0); 
	EmitVertex();
	EndPrimitive();

	if(gl_in[0].gl_Position.z != 0)
	{
		//Получаем масштаб
		float	pix		= 1.0/worldToCamera[0][0];

		//Ограничиваем минимальный штрих одним пикселем
		vec2	begin	= gl_in[0].gl_Position.xy;
		vec2	end		= gl_in[1].gl_Position.xy;
		if(end.x - begin.x < pix)
			end.x	= begin.x + pix;

		//Верхняя линия
		gl_Position 	= cameraToView * worldToCamera * vec4(begin + vec2(0,pix), 0.0, 1.0); 
		EmitVertex();
		gl_Position 	= cameraToView * worldToCamera * vec4(end + vec2(0,pix), 0.0, 1.0); 
		EmitVertex();
		EndPrimitive();

		//Нижняя линия
		gl_Position 	= cameraToView * worldToCamera * vec4(begin - vec2(0,pix), 0.0, 1.0);
		EmitVertex();
		gl_Position 	= cameraToView * worldToCamera * vec4(end - vec2(0,pix), 0.0, 1.0);
		EmitVertex();
		EndPrimitive();
	}
}  

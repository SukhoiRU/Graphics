#version 330 core
layout (lines) in;
layout (triangle_strip, max_vertices = 8) out;

uniform mat4 worldToCamera;
uniform mat4 cameraToView;

uniform int lineType;
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

	switch(lineType)
	{
		case 1:
		{
			//Ступеньки
			float	len		= gl_in[1].gl_Position.x - gl_in[0].gl_Position.x;
			float	dL		= 0.5*linewidth + 1.5*antialias;
			
			//Горизонтальный штрих
			vec2	d1		= vec2(0., -dL);
			vec2	d2		= vec2(-dL, 0);

			gs_out.L		= len;
			
			gs_out.coord	= vec2(-dL, -dL);
			gl_Position = cameraToView *  worldToCamera * (gl_in[0].gl_Position + vec4(-d1+d2, 0., 0.));;
			EmitVertex();

			gs_out.coord	= vec2(-dL, dL);
			gl_Position = cameraToView *  worldToCamera * (gl_in[0].gl_Position + vec4(d1+d2, 0., 0.)); 
			EmitVertex();

			gs_out.coord	= vec2(len+dL, -dL);
			gl_Position = cameraToView *  worldToCamera * (vec4(gl_in[1].gl_Position.x, gl_in[0].gl_Position.y, 0., 1.0)  + vec4(-d1-d2, 0., 0.)); 
			EmitVertex();

			gs_out.coord	= vec2(len+dL, dL);
			gl_Position = cameraToView *  worldToCamera * (vec4(gl_in[1].gl_Position.x, gl_in[0].gl_Position.y, 0., 1.0) + vec4(d1-d2, 0., 0.)); 
			EmitVertex();
			EndPrimitive();

			//Вертикальный штрих
			len		= abs(gl_in[1].gl_Position.y - gl_in[0].gl_Position.y);				
			if(gl_in[1].gl_Position.y > gl_in[0].gl_Position.y)	
			{
				d1		= vec2(dL, 0.);	
				d2		= vec2(0., -dL);
			}
			else
			{
				d1		= vec2(-dL, 0.);	
				d2		= vec2(0., dL);
			}												

			gs_out.L		= len;
			
			gs_out.coord	= vec2(-dL, -dL);
			gl_Position = cameraToView *  worldToCamera * (vec4(gl_in[1].gl_Position.x, gl_in[0].gl_Position.y, 0., 1.0) + vec4(-d1+d2, 0., 0.)); 
			EmitVertex();

			gs_out.coord	= vec2(-dL, dL);
			gl_Position = cameraToView *  worldToCamera * (vec4(gl_in[1].gl_Position.x, gl_in[0].gl_Position.y, 0., 1.0) + vec4(d1+d2, 0., 0.)); 
			EmitVertex();

			gs_out.coord	= vec2(len+dL, -dL);
			gl_Position = cameraToView *  worldToCamera * (gl_in[1].gl_Position + vec4(-d1-d2, 0., 0.)); 
			EmitVertex();

			gs_out.coord	= vec2(len+dL, dL);
			gl_Position = cameraToView *  worldToCamera * (gl_in[1].gl_Position + vec4(d1-d2, 0., 0.)); 
			EmitVertex();
			EndPrimitive();
		}break;

	default:
		{
			//Нормальная линия
			vec2	v0	= (gl_in[1].gl_Position.xy- gl_in[0].gl_Position.xy);
			
			float len	= length(v0);
			v0	= normalize(v0);

			float	dL		= 0.5*linewidth + 1.5*antialias;
			vec2	d1		= dL*vec2(v0.y, -v0.x);
			vec2	d2		= -v0*dL;
			gs_out.L		= len;
			
			gs_out.coord	= vec2(-dL, -dL);
			gl_Position = cameraToView *  worldToCamera * (gl_in[0].gl_Position + vec4(-d1+d2, 0., 0.)); 
			EmitVertex();

			gs_out.coord	= vec2(-dL, dL);
			gl_Position = cameraToView *  worldToCamera * (gl_in[0].gl_Position + vec4(d1+d2, 0., 0.)); 
			EmitVertex();

			gs_out.coord	= vec2(len+dL, -dL);
			gl_Position = cameraToView *  worldToCamera * (gl_in[1].gl_Position + vec4(-d1-d2, 0., 0.)); 
			EmitVertex();

			gs_out.coord	= vec2(len+dL, dL);
			gl_Position = cameraToView *  worldToCamera * (gl_in[1].gl_Position + vec4(d1-d2, 0., 0.)); 
			EmitVertex();
			EndPrimitive();
		}break;
	}
}  

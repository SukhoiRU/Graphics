#version 330 core
layout (lines_adjacency) in;
layout (triangle_strip, max_vertices = 8) out;

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
	flat float	tg1;
	flat float	tg2;
	flat vec3	c;		//Коэффициенты полинома
	// flat vec2	sin_cos_1;
	// flat vec2	sin_cos_2;
}gs_out;

float	cross(vec2 v1, vec2 v2)
{
	return v1.x*v2.y - v1.y*v2.x;
}

void main() 
{ 
	gs_out.gColor = vColor[0];

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
				EmitVertex();
				gl_Position 	= vec4(gl_in[0].gl_Position.x, baseLine, 0., 1.);
				EmitVertex();
				
				//Черта в нуле
				EndPrimitive();
				gl_Position 	= vec4(gl_in[0].gl_Position.x, baseLine, 0., 1.);
				EmitVertex();
				gl_Position 	= vec4(gl_in[1].gl_Position.x, baseLine, 0., 1.);
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
			//Вычисляем угол с предыдущей линией
			vec2	v0	= (gl_in[1].gl_Position.xy- gl_in[0].gl_Position.xy);
			vec2	v1	= (gl_in[2].gl_Position.xy- gl_in[1].gl_Position.xy);
			vec2	v2	= (gl_in[3].gl_Position.xy- gl_in[2].gl_Position.xy);
			
			float len	= length(v1);
			v0	= normalize(v0);
			v1	= normalize(v1);
			v2	= normalize(v2);

			float	cos0	= v1.x;//dot(v1, v0);
			float	sin0	= v1.y;//cross(vec3(v1, 0.), vec3(v0,0.)).z;
			float	dL		= (0.5*linewidth + 1.5*antialias);
			vec2	d1		= dL*vec2(sin0, -cos0);
			vec2	d2		= -v1*dL;

			float	cos1	= dot(v1, v0);
			float	sin1	= cross(v1, v0);
			gs_out.tg1	= sin1/(1.0+cos1);
			// if(gs_out.tg1 > 0)
			// 	gs_out.sin_cos_1	= vec2(sqrt(0.5*(1.-cos1)), sqrt(0.5*(1.+cos1)));
			// else
			// 	gs_out.sin_cos_1	= vec2(-sqrt(0.5*(1.-cos1)), sqrt(0.5*(1.+cos1)));


			float	cos2	= dot(v1, v2);
			float	sin2	= cross(v1, v2);
			gs_out.tg2	= sin2/(1.0+cos2);
			// if(gs_out.tg2 > 0)
			// 	gs_out.sin_cos_1	= vec2(sqrt(0.5*(1.-cos2)), sqrt(0.5*(1.+cos2)));
			// else
			// 	gs_out.sin_cos_1	= vec2(-sqrt(0.5*(1.-cos2)), sqrt(0.5*(1.+cos2)));

			//Расчет коэффициентов полинома
			gs_out.c.z		= gs_out.tg1;
			gs_out.c.y		= (-gs_out.tg2 - 2.*gs_out.tg1)/len;
			gs_out.c.x		= (gs_out.tg2 + gs_out.tg1)/len/len;

			gs_out.L		= len;
			
			gs_out.coord	= vec2(-dL, -dL);
			gl_Position = cameraToView *  worldToCamera * (gl_in[1].gl_Position + vec4(-d1+d2, 0., 0.)); 
			EmitVertex();

			gs_out.coord	= vec2(-dL, dL);
			gl_Position = cameraToView *  worldToCamera * (gl_in[1].gl_Position + vec4(d1+d2, 0., 0.)); 
			EmitVertex();

			gs_out.coord	= vec2(len+dL, -dL);
			gl_Position = cameraToView *  worldToCamera * (gl_in[2].gl_Position + vec4(-d1-d2, 0., 0.)); 
			EmitVertex();

			gs_out.coord	= vec2(len+dL, dL);
			gl_Position = cameraToView *  worldToCamera * (gl_in[2].gl_Position + vec4(d1-d2, 0., 0.)); 
			EmitVertex();
			EndPrimitive();
		}break;
	}
}  

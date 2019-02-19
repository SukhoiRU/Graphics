#version 330 core
layout (lines_adjacency) in;
layout (triangle_strip, max_vertices = 6) out;

flat in vec4 vColor[];
flat out vec4 gColor;

uniform mat4 worldToCamera;
uniform mat4 cameraToView;
uniform int round;

uniform int lineType;
uniform float baseLine;
uniform vec2 pixelSize;

uniform float linewidth, antialias;

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
			
			float	len	= length(v1);
			v0	= normalize(v0);
			v1	= normalize(v1);
			v2	= normalize(v2);

			float	cos1	= v1.x;//dot(v1, v0);
			float	sin1	= v1.y;//cross(vec3(v1, 0.), vec3(v0,0.)).z;
			vec4	dL		= linewidth*vec4(sin1, -cos1, 0.,0.);

			gl_Position = cameraToView *  worldToCamera * (gl_in[1].gl_Position - dL); 
			EmitVertex();
			gl_Position = cameraToView *  worldToCamera * (gl_in[1].gl_Position + dL); 
			EmitVertex();
			gl_Position = cameraToView *  worldToCamera * (gl_in[2].gl_Position - dL); 
			EmitVertex();
			gl_Position = cameraToView *  worldToCamera * (gl_in[2].gl_Position + dL); 
			EmitVertex();
			EndPrimitive();
		}break;
	}
}  

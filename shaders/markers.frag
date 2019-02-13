#version 330 core

const float PI = 3.14159265358979323846264;
const float M_SQRT_2 = 1.4142135623730951;

uniform float size, linewidth, antialias;
uniform vec4 fg_color, bg_color;

flat in vec2 rotation;
flat in float v_size;
out vec4 fColor;

vec4 stroke(float distance,  // Signed distance to line
            float linewidth, // Stroke line width
            float antialias, // Stroke antialiased area
            vec4 stroke)     // Stroke color
{
  float t = linewidth / 2.0 - antialias;
  float signed_distance = distance;
  float border_distance = abs(signed_distance) - t;
  float alpha = border_distance / antialias;
  alpha = exp(-alpha * alpha);

	if(border_distance > (linewidth/2. + antialias))	discard;
	else if (border_distance < 0.0)	return stroke;
  else														return vec4(stroke.rgb, stroke.a * alpha);
}

vec4 filled(float distance,  // Signed distance to line
            float linewidth, // Stroke line width
            float antialias, // Stroke antialiased area
            vec4 fill)       // Fill color
{
  float t = linewidth / 2.0 - antialias;
  float signed_distance = distance;
  float border_distance = abs(signed_distance) - t;
  float alpha = border_distance / antialias;
  alpha = exp(-alpha * alpha);

  if (border_distance < 0.0)			return fill;	//Внутри обводки
  else if (signed_distance < 0.0)	return fill;	//Внутри фигуры
  else 
	{
		//Снаружи фигуры
		if(border_distance > (linewidth/2. + antialias))	discard;
		else	return vec4(fill.rgb, alpha * fill.a);
  }
                
	
}

vec4 outline(float distance,  // Signed distance to line
             float linewidth, // Stroke line width
             float antialias, // Stroke antialiased area
             vec4 stroke,     // Stroke color
             vec4 fill)       // Fill color
{
	float t = linewidth / 2.0 - antialias;
	float signed_distance = distance;
	float border_distance = abs(signed_distance) - t;
	float alpha = border_distance / antialias;
	alpha = exp(-alpha * alpha);

	if (border_distance < 0.0)			return stroke;
	else if (signed_distance < 0.0)
	{
		//Внутри фигуры
		if(border_distance > (linewidth/2. + antialias))	return fill;
		else																							return mix(fill, stroke, sqrt(alpha));
	}	
	else
	{
		//Снаружи фигуры
		if(border_distance > (linewidth/2. + antialias))	discard;
		else																							return vec4(stroke.rgb, stroke.a * alpha);
	}
}

float disc(vec2 P, float size) 
{
	return length(P) - size / 2; 
}

float square(vec2 P, float size) 
{
	return max(abs(P.x), abs(P.y)) - size / (2.0 * M_SQRT_2);
}

float triangle(vec2 P, float size)
{
	float x = M_SQRT_2/2.0 * (P.x - P.y);
	float y = M_SQRT_2/2.0 * (P.x + P.y);
	float r1 = max(abs(x), abs(y)) - size/(2*M_SQRT_2);
	float r2 = P.y;
	return max(r1,r2);
}

float diamond(vec2 P, float size)
{
	float x = M_SQRT_2/2.0 * (P.x - P.y);
	float y = M_SQRT_2/2.0 * (P.x + P.y);
	return max(abs(x), abs(y)) - size/(2.0*M_SQRT_2);
}

float heart(vec2 P, float size) 
{
  float x = M_SQRT_2 / 2.0 * (P.x - P.y);
  float y = M_SQRT_2 / 2.0 * (P.x + P.y);
  float r1 = max(abs(x), abs(y)) - size / 3.5;
  float r2 = length(P - M_SQRT_2 / 2.0 * vec2(+1.0, -1.0) * size / 3.5) - size / 3.5;
  float r3 = length(P - M_SQRT_2 / 2.0 * vec2(-1.0, -1.0) * size / 3.5) - size / 3.5;
  return min(min(r1, r2), r3);
}

float chevron(vec2 P, float size)
{
	float x = 1.0/M_SQRT_2 * (P.x - P.y);
	float y = 1.0/M_SQRT_2 * (P.x + P.y);
	float r1 = max(abs(x), abs(y)) - size/3.0;
	float r2 = max(abs(x-size/3.0), abs(y-size/3.0)) - size/3.0;
	return max(r1,-r2);
}

float clover(vec2 P, float size) 
{
	const float PI = 3.14159265358979323846264;
	const float t1 = -PI / 2;
	const vec2 c1 = 0.25 * vec2(cos(t1), sin(t1));
	const float t2 = t1 + 2 * PI / 3;
	const vec2 c2 = 0.25 * vec2(cos(t2), sin(t2));
	const float t3 = t2 + 2 * PI / 3;
	const vec2 c3 = 0.25 * vec2(cos(t3), sin(t3));
	float r1 = length(P - c1 * size) - size / 3.5;
	float r2 = length(P - c2 * size) - size / 3.5;
	float r3 = length(P - c3 * size) - size / 3.5;
	return min(min(r1, r2), r3);
}

float ring(vec2 P, float size)
{
	float r1 = length(P) - size/2.0;
	float r2 = length(P) - size/4.0;
	return max(r1,-r2);
}

float cross(vec2 P, float size)
{
	float x = M_SQRT_2/2.0 * (P.x - P.y);
	float y = M_SQRT_2/2.0 * (P.x + P.y);
	float r1 = max(abs(x - size/3.0), abs(x + size/3.0));
	float r2 = max(abs(y - size/3.0), abs(y + size/3.0));
	float r3 = max(abs(x), abs(y));
	return max(min(r1,r2),r3) - size/2.0;
}

float asterisk(vec2 P, float size)
{
	float x = M_SQRT_2/2.0 * (P.x - P.y);
	float y = M_SQRT_2/2.0 * (P.x + P.y);
	float r1 = max(abs(x)- size/2.0, abs(y)- size/10.0);
	float r2 = max(abs(y)- size/2.0, abs(x)- size/10.0);
	float r3 = max(abs(P.x)- size/2.0, abs(P.y)- size/10.0);
	float r4 = max(abs(P.y)- size/2.0, abs(P.x)- size/10.0);
	return min( min(r1,r2), min(r3,r4));
}

float pin(vec2 P, float size) 
{
	vec2 c1 = vec2(0.0,-0.15)*size;
	float r1 = length(P-c1)-size/2.675;
	vec2 c2 = vec2(+1.49,-0.80)*size;
	float r2 = length(P-c2) - 2.*size;
	vec2 c3 = vec2(-1.49,-0.80)*size;
	float r3 = length(P-c3) - 2.*size;
	float r4 = length(P-c1)-size/5;
	return max( min(r1,max(max(r2,r3),-P.y)), -r4);
}

float axeGrid(vec2 P, float size)
{
    float dx1	= 0.75/5.*size;
    float dx2	= 1.5/5.*size;
    float dy = 0.5 * size / 5.;

    //Вертикальная линия
    float	v	= max(abs(P.x), max(P.y-0.5*size, -0.5*size-P.y));
    
    //Нижний штрих
    float	d	= max(abs(P.y-0.5*size), max(P.x-0., -dx2-P.x));
    float	r	= min(v,d);
    
    //Короткий штрих
    for(float i = 2.*dy; i < 10.*dy; i+= 2.*dy)
    {
	    d	= max(abs(P.y-0.5*size+i), max(P.x, -dx1-P.x));    
    	r = min(r, d);
    }

		//Верхний штрих
    d	= max(abs(P.y+0.5*size), max(P.x-0., -dx2-P.x));
		r = min(r, d);

    return r;
}

void main() 
{
	vec2 P = gl_PointCoord.xy - vec2(0.5, 0.5);
	P = vec2(rotation.x * P.x - rotation.y * P.y,
			rotation.y * P.x + rotation.x * P.y);
	float distance = axeGrid(P * v_size, size);
	fColor = stroke(distance, linewidth, antialias, bg_color);//, bg_color);
}

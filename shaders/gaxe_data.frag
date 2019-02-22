#version 330 core

in GS_OUT
{
	vec2 coord;			//Координаты углов в мм относительно линии
	flat vec4 	gColor;
	flat float	L;		//Длина отрезка
 	flat float	bValue;
	// flat float	tg1;
	// flat float	tg2;
	// flat vec3	  c;		//Коэффициенты полинома
	// flat vec2	  sin_cos_1;
	// flat vec2	  sin_cos_2;
}gs_in;

out vec4 fColor;

uniform int lineType;
uniform float linewidth, antialias;

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

void main()
{
  //Расстояние до отрезка
  float distance = gs_in.coord.y;

  //Расстояние до полинома
  // float x   = gs_in.coord.x;
  // float y   = gs_in.c.x*x*x*x + gs_in.c.y*x*x + gs_in.c.z*x;
  // float dy  = 3.*gs_in.c.x*x*x + 2.*gs_in.c.y*x + gs_in.c.z;
  //distance  = y*cos(atan(dy));
  //distance  = (y - gs_in.coord.y);//*sin(atan(dy));


  //Расстояние до левой границы стыковки
  // float d1  = gs_in.coord.x*gs_in.sin_cos_1.y + gs_in.coord.y*gs_in.sin_cos_1.x;
  // float d2  = (gs_in.coord.x-gs_in.L)*gs_in.sin_cos_2.y + gs_in.coord.y*gs_in.sin_cos_2.x;
  // d2  = min(d1, d2);

  //Обрезаем ближний конец
  //if(gs_in.coord.x < gs_in.coord.y*gs_in.tg1)  discard;
 
  //Обрезаем дальний конец
  //if(gs_in.coord.x > gs_in.L - gs_in.coord.y*gs_in.tg2)  discard;

  //Полукруги возле концов
  if(gs_in.coord.x < 0.)
    distance = length(gs_in.coord);
  else if(gs_in.coord.x > gs_in.L)
    distance = length(gs_in.coord - vec2(gs_in.L, 0.));

  // if(gs_in.coord.x < gs_in.coord.y*gs_in.tg1)  distance = 10.;
  // if(gs_in.coord.x > gs_in.L - gs_in.coord.y*gs_in.tg2)  distance = 10.;
 
  if(lineType == 3)
    {
      if(gs_in.bValue > 0.)
        fColor = stroke(distance, linewidth, 0.*antialias, gs_in.gColor);
      else
        fColor = stroke(distance, 0.333*linewidth, 0.*antialias, gs_in.gColor);      
    }
  else
    fColor = stroke(distance, linewidth, antialias, gs_in.gColor);

//  fColor  = gs_in.gColor;
  //if(fColor.a > 0.5)   fColor.a = 0.5;
//   if(fColor.a < 0.05)   fColor.a = 0.05;
}
  

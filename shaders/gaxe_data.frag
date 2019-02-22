#version 330 core

in vec2	coord;	   //Координаты углов в мм относительно линии
flat in vec4 gColor;
flat in float	L;		//Длина отрезка
flat in float	tg1;
flat in float	tg2;

out vec4 fColor;

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
  float distance = coord.y;

  //Обрезаем ближний конец
  if(tg1 > 0 && (coord.y > coord.x*tg1))  discard;
  if(tg1 < 0 && (coord.y < coord.x*tg1))  discard;

  //Обрезаем дальний конец
 // if(tg2 < 0 && (coord.y > (coord.x-L)*tg2))  discard;
 // if(tg2 > 0 && (coord.y < (coord.x-L)*tg2))  discard;

  //Полукруги возле концов
  if(coord.x < 0.)
    distance = length(coord);
  else if(coord.x > L)
    distance = length(coord - vec2(L, 0.));
  
  fColor = stroke(distance, linewidth, antialias, gColor);
  if(fColor.a > 0.5)   fColor.a = 0.5;
//   if(fColor.a < 0.05)   fColor.a = 0.05;
}
  
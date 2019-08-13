#version 330 core

in GS_OUT
{
	vec2 coord;			//Координаты углов в мм относительно линии
	flat vec4 	gColor;
	flat float	L;		//Длина отрезка
	flat float	bValue;
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
  alpha = exp(-2.0*alpha * alpha);

	/*if(border_distance > (linewidth/2. + antialias))	discard;
	else*/ if (border_distance < 0.0)	return stroke;
  else														return vec4(stroke.rgb, stroke.a * alpha);
}

void main() 
{
  if (lineType == 3) 
  {
    //Отрисовка bool
    fColor = gs_in.gColor;
    return;
  }

  //Расстояние до отрезка
  float distance = abs(gs_in.coord.y);
  float Kalpha = 1;

  //Полукруги возле концов
  if (gs_in.coord.x < 0.) 
  {
    distance = length(gs_in.coord);
    Kalpha = 0.5;
  }
  else if (gs_in.coord.x > gs_in.L) 
  {
    distance = length(gs_in.coord - vec2(gs_in.L, 0.));
    Kalpha = 0.5;
  }

  fColor = stroke(distance, linewidth, antialias, gs_in.gColor);
  fColor.a *= Kalpha;
   //fColor  = gs_in.gColor;
  // if(fColor.a > 0.5)   fColor.a = 0.5;
  //   if(fColor.a < 0.05)   fColor.a = 0.05;
}

#version 330 core
in vec3 TexCoords;
out vec4 color;

uniform sampler2DArray msdf;
uniform float pxRange;
uniform vec3 textColor;

float median(float r, float g, float b) {return max(min(r, g), min(max(r, g), b));}

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
// 	vec3 flipped_texCoords = TexCoords;//vec3(TexCoords.x, 1.0 - TexCoords.y, TexCoords.z);
//     vec2 pos = flipped_texCoords.xy;
//     vec3 sample = texture(msdf, flipped_texCoords).rgb;
//     ivec2 sz = textureSize(msdf, 0).xy;
//     float dx = dFdx(pos.x) * sz.x; 
//     float dy = dFdy(pos.y) * sz.y;
//     float toPixels = 8.0 * inversesqrt(dx * dx + dy * dy);
//     float sigDist = median(sample.r, sample.g, sample.b);
// //	color = stroke(sigDist-0.5, 1.0, 0.5, vec4(textColor, 1.0));
//     float w = fwidth(sigDist);
//     float opacity = smoothstep(0.5 - w, 0.5 + w, sigDist);
//     color = vec4(textColor, opacity);

  vec2 msdfUnit = pxRange / vec2(textureSize(msdf, 0));
  vec3 sample = texture(msdf, TexCoords).rgb;
  float sigDist = median(sample.r, sample.g, sample.b) - 0.5;
  // color = stroke(sigDist, 1.0, 0.5, vec4(textColor, 1.0));
  //vec2  ddX = fwidth(TexCoords.xy);
  //if(ddX != vec2(0.0))
  sigDist *= dot(msdfUnit, 0.5 / fwidth(TexCoords.xy));
  float opacity = clamp(sigDist + 0.5, 0.0, 1.0);
  if(opacity < 0.05) opacity = 0.05;
  color = vec4(textColor, opacity);
//      color = mix(vec4(1.0,1.,1.,1.0), vec4(textColor,1.0), opacity);
}

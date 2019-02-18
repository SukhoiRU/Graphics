#version 330 core
in vec4 TexCoords;
out vec4 color;

uniform sampler2DArray msdf;
uniform float pxRange;
uniform vec3 textColor;

float median(float r, float g, float b) {return max(min(r, g), min(max(r, g), b));}

void main()
{
  // vec3 flipped_texCoords = TexCoords.xyz; // vec3(TexCoords.x, 1.0 - TexCoords.y, TexCoords.z);
  // vec2 pos = flipped_texCoords.xy;
  // vec3 sample = texture(msdf, flipped_texCoords).rgb;
  // ivec2 sz = textureSize(msdf, 0).xy;
  // float dx = dFdx(pos.x) * sz.x;
  // float dy = dFdy(pos.y) * sz.y;
  // float toPixels = pxRange*8.0 * inversesqrt(dx * dx + dy * dy);
  // float sigDist = median(sample.r, sample.g, sample.b) - 0.5;
  // float opacity = clamp( sigDist * toPixels + 0.5, 0.0, 1.0 );
  // //float w = fwidth(sigDist);
  // //float opacity = smoothstep(0.5 - w, 0.5 + w, sigDist);
  // color = vec4(textColor, opacity);

  vec2 msdfUnit = vec2(TexCoords.w)*0. + pxRange / vec2(textureSize(msdf, 0));
  vec3 sample = texture(msdf, TexCoords.xyz).rgb;
  float sigDist = median(sample.r, sample.g, sample.b) - 0.5;
  sigDist *= dot(msdfUnit, 0.5 / fwidth(TexCoords.xy));
  float opacity = clamp(sigDist + 0.5, 0.0, 1.0);
  //if(opacity < 0.05) opacity = 0.05;
  color = vec4(textColor, opacity);
}

#version 330 core

const float M_SQRT_2 = 1.4142135623730951;

uniform mat4 ortho;
uniform float size, orientation, linewidth, antialias;

layout(location=0) in vec2 position;
out vec2 rotation;
out vec2 v_size;

void main(void) 
{
  rotation      = vec2(cos(orientation), sin(orientation));
  gl_Position   = ortho * vec4(position, 0.0, 1.0);
  v_size        = vec2(M_SQRT_2 * size + 2.0 * (linewidth + 1.5 * antialias));
  gl_PointSize  = v_size.x;
}

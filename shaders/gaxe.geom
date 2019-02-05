#version 330 core
layout (points) in;
layout (line_strip, max_vertices = 2) out;

in vec4 vColor[];
out vec4 gColor;

void main() {    
	gColor = vColor[0];
    gl_Position = gl_in[0].gl_Position + vec4(0.0, 0.01, 0.0, 0.0); 
    EmitVertex();

    gl_Position = gl_in[0].gl_Position - vec4(0.0, 0.01, 0.0, 0.0); 
    EmitVertex();

    EndPrimitive();
}  

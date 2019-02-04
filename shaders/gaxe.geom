#version 330 core
layout (lines) in;
layout (line_strip, max_vertices=2)  out;

uniform int lineType;

void main()
{
    for ( int i = 0; i < gl_in.length (); i++ )
    {
        gl_Position = gl_in [i].gl_Position;
        EmitVertex ();
    }
	
    EndPrimitive ();
}

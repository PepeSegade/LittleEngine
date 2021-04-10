#version 330

layout(triangles) in;
layout(points, max_vertices = 4) out;

void main()
{
	for(int i = 0; i < 4; ++i)
	{
		gl_Position = gl_in[i % 3].gl_Position;
		EmitVertex();
	}

	EndPrimitive();
}
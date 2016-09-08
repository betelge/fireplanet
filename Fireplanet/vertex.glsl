#version 120
#extension GL_EXT_gpu_shader4 : require

varying vec2 coord;

void main(void)
{
	/*
		This vertex shader will only be used for full screen triangles
		drawn with glDrawArray(GL_TRIANGLES, 0, 3) without any vertex
		data. gl_VertexID is the only input. No depth buffer is used
		*/

	vec2 texcoord = vec2((gl_VertexID << 1) & 2, gl_VertexID & 2);
	vec3 vertex = vec3(texcoord * vec2(-2., -2.) + vec2(1., 1.), 1.);

	coord = vertex.xy; // Pass screen coordinates to fragment shader.
    gl_Position = vec4(vertex.xy, .5, 1.); // Set z = 0.5. 
}

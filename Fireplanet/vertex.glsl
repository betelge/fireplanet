#version 120
#extension GL_EXT_gpu_shader4 : require

out vec2 coord;

void main(void)
{
	vec2 texcoord = vec2((gl_VertexID << 1) & 2, gl_VertexID & 2);
	vec3 vertex = vec3(texcoord * vec2(-2., -2.) + vec2(1., 1.), 1.);

	coord = vertex.xy;
    gl_Position = vec4(vertex.xy, .5, 1.);
}

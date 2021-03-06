#version 330

layout(location=0) in vec3 my_vertex_position;
layout(location = 1) in vec2 vertex_uv0;
layout(location = 2) in vec3 vertex_normal;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;

out vec2 uv0;
out vec3 normal;
out vec3 position;

void main()
{
	uv0 = vertex_uv0;
	normal = transpose(inverse(mat3(model))) * vertex_normal;
	vec4 pos = model * vec4(my_vertex_position, 1.0);
	position = vec3(pos);
	gl_Position = proj * view * pos;
}

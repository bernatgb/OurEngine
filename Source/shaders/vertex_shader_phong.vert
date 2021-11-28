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
	gl_Position = proj * view * model * vec4(my_vertex_position, 1.0);
	uv0 = vertex_uv0;
	normal = vec3(view * transpose(inverse(model)) * vec4(vertex_normal, 0.0));
	vec4 pos = view * model * vec4(my_vertex_position, 1.0);
	position = vec3(pos) / pos.w;
}

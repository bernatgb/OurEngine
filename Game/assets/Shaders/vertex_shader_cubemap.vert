#version 330

layout(location=0) in vec3 pos;

uniform mat4 proj;
uniform mat4 view;

out vec3 texCoords;

void main()
{
	texCoords = pos;
	vec4 auxPos = proj * view * vec4(pos, 1.0);
	//gl_Position = proj * view * vec4(pos, 1.0);
	gl_Position = auxPos.xyww - vec4(0.0, 0.0, 0.0001, 0.0);
}

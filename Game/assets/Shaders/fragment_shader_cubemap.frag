#version 330

in vec3 texCoords;

uniform samplerCube skybox;

out vec4 color;

void main()
{
	color = texture(skybox, texCoords);
}

#version 330

in vec2 uv0;
in vec3 normal;
in vec3 position;

uniform sampler2D texture;
uniform vec3 light_dir;
uniform vec3 cam_pos;

uniform vec3 cam_pos;
uniform vec3 color_a;
uniform vec3 color_l;

out vec4 color;

void main()
{
	vec3 N = normalize(normal);
	vec3 V = normalize(cam_pos - position);
	vec3 L = light_dir;

	float lambert = -dot(N, L);

	if (lambert >= 0.0) 
	{
		color = texture2D(texture, uv0) * lambert;
	}
	else
	{
		//color = texture2D(texture, uv0);
		color = vec4(0.0, 0.0, 0.0, 1.0);
	}

	//color = texture2D(texture, uv0);

	//color_a*color_d+kd*color_d*color_l*(-N*L)+ks*color_l*pow((V*R), n)
}

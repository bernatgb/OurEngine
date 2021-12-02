#version 330

in vec2 uv0;
in vec3 normal;
in vec3 position;

uniform mat4 view;

uniform sampler2D texture;
uniform vec3 light_pos;
uniform vec3 cam_pos;

uniform vec3 color_a;
uniform vec3 color_l;

uniform float kd;
uniform float ks;
uniform float n;

out vec4 color;

void main()
{
	vec3 N = normalize(normal);
	vec3 L = normalize(position - light_pos);
	vec3 V = normalize(cam_pos - position);
	vec3 R = normalize(reflect(L, N));
	//vec3 R = L - 2 * dot(N, L) * N;

	float lambert = -dot(N, L);
	vec3 color_d = vec3(texture2D(texture, uv0));
	vec3 aux_color = color_a * color_d;

	if (lambert >= 0.0) 
	{
		aux_color += (kd * color_d * color_l * lambert);
		aux_color += (ks * color_l * pow(dot(V, R), n));
	}
	color = vec4(aux_color, 1.0);

	//color_a*color_d+kd*color_d*color_l*(-N*L)+ks*color_l*pow((V*R), n)
}

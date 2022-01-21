#version 330

struct Light
{
 int lightType;
 vec3 lightColor;
 float intensity;
 float radius;
 float innerAngle;
 float outerAngle;
 
 vec3 direction;
 vec3 position;
};

#define PI 3.1415926538

in vec2 uv0;
in vec3 normal;
in vec3 position;

uniform mat4 view;

// MATERIAL
uniform vec3 diffuseColor;
uniform sampler2D diffuseTex;
uniform vec3 specularColor;
uniform sampler2D specularTex;
uniform float shininess;

uniform bool hasDiffuseTex;
uniform bool hasSpecularTex;
uniform bool shininessAlpha;
//

uniform vec3 ambientColor;

uniform vec3 camPos;

out vec4 color;

vec3 GetPhongBRDFDirectional(Light light, vec3 Cs, vec3 N, vec3 V, float n, vec3 part1)
{
	vec3 L = light.direction;
	vec3 R = reflect(L, N);
	vec3 Li = light.lightColor * light.intensity;
	float lambert = max(dot(N, -L), 0.0);
	vec3 Rf = Cs + (1.0 - Cs) * pow(1.0 - lambert, 5.0);
	
	return (part1 + ((n + 2.0)/2.0) * Rf * pow(max(dot(V, R), 0.0001), n)) * Li * lambert;
}

vec3 GetPhongBRDFPoint(Light light, vec3 Cs, vec3 N, vec3 V, float n, vec3 pos, vec3 part1) 
{
	float dist = distance(light.position, pos);
	float Fatt = pow(max(1 - pow(dist / light.radius, 4.0), 0.0), 2.0)/(dist * dist + 1);
	vec3 L = normalize(pos - light.position);
	vec3 R = reflect(L, N);
	vec3 Li = light.lightColor * light.intensity * Fatt;
	float lambert = max(-dot(N, L), 0.0);
	vec3 Rf = Cs + (1.0 - Cs) * pow(1.0 - lambert, 5.0);
	
	return (part1 + ((n + 2.0)/2.0) * Rf * pow(max(dot(V, R), 0.0001), n)) * Li * lambert;
}

vec3 GetPhongBRDFSpot(Light light, vec3 Cs, vec3 N, vec3 V, float n, vec3 pos, vec3 part1) 
{
	vec3 L = normalize(pos - light.position);
	float C = dot(L, light.direction);
	float Catt = 0;
	if (C >= cos(light.innerAngle))
		Catt = 1;
	else if (C < cos(light.innerAngle) && C >= cos(light.outerAngle))
		Catt = (C - cos(light.outerAngle))/(cos(light.innerAngle) - cos(light.outerAngle));
		
	float dist = dot(L, light.direction);
	float Fatt = pow(max(1 - pow(dist / light.radius, 4.0), 0.0), 2.0)/(dist * dist + 1);
	vec3 R = reflect(L, N);
	vec3 Li = light.lightColor * light.intensity * Fatt * Catt;
	float lambert = max(-dot(N, L), 0.0);
	vec3 Rf = Cs + (1.0 - Cs) * pow(1.0 - lambert, 5.0);
	
	return (part1 + ((n + 2.0)/2.0) * Rf * pow(max(dot(V, R), 0.0001), n)) * Li * lambert;
}

void main()
{    
    vec3 N = normalize(normal);
    vec3 V = normalize(camPos - position);
    
    vec3 Ca = ambientColor;
    vec3 Cd = diffuseColor;
    if (hasDiffuseTex) Cd = texture(diffuseTex, uv0).rgb;
    Cd = pow(Cd, vec3(2.2));

    vec3 Cs = specularColor;
    if (hasSpecularTex) Cs = texture(specularTex, uv0).rgb;
    
    float n = shininess;
    if (shininessAlpha) n = pow(2, texture(specularTex, uv0).a * 7 + 1);
    
 	vec3 part1 = (Cd * (1.0 - Cs));
 	
 	Light light;
 	light.lightType = 3;
 	
 	vec3 finalColor = vec3(0.0, 0.0, 0.0);
    if (light.lightType == 0)
        finalColor += GetPhongBRDFDirectional(light, Cs, N, V, n, part1);
    else if (light.lightType == 1)
    	finalColor += GetPhongBRDFPoint(light, Cs, N, V, n, position, part1);
	else if (light.lightType == 2)
    	finalColor += GetPhongBRDFSpot(light, Cs, N, V, n, position, part1);
    
    finalColor += Ca * Cd;
    finalColor = finalColor / (finalColor + vec3(1.0)); // reinhard tone mapping
    
    color = vec4(pow(finalColor, vec3(1.0/2.2)), 1.0); // gamma correction
}

/*
// Ca - ambient light
// Cd - difuse color
// Cs (Rf(0)) - specular color
// Li - light color
// n - shininess
// Rf(0i) ~= Rf(0) + 1 * (1 - Rf(0))(1-cos0i)^5 - fresnel equation
// Rf(0) (being n the index of refraction) = ((n-1)/(n+1))^2
*/
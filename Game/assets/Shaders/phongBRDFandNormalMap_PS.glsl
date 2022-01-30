#version 330

struct Light
{
 int lightType;
 vec3 lightColor;
 vec4 values;
 //float intensity;
 //float radius;
 //float innerAngle;
 //float outerAngle;
 
 vec3 direction;
 vec3 position;
};

#define PI 3.1415926538
#define MAX_NUM_TOTAL_LIGHTS 20

in vec2 uv0;
in vec3 normal;
in vec3 tangent;
in vec3 position;

uniform mat4 view;

// MATERIAL
uniform vec3 diffuseColor;
uniform sampler2D diffuseTex;
uniform vec3 specularColor;
uniform sampler2D specularTex;
uniform float shininess;
uniform sampler2D normalMap;

uniform bool hasDiffuseTex;
uniform bool hasSpecularTex;
uniform bool shininessAlpha;
uniform bool hasNormalMap;
//

layout (std140) uniform Lights
{
 int numLights; 
 Light[MAX_NUM_TOTAL_LIGHTS] lights; 
};

uniform vec3 ambientColor;

uniform vec3 camPos;

out vec4 color;

mat3 CreateTangentSpace(const vec3 normal, const vec3 tangent)
{
	vec3 ortho_tangent = normalize(tangent - dot(tangent, normal) * normal); // Gram-Schmidt
	vec3 bitangent = cross(normal, ortho_tangent);
	return mat3(tangent, bitangent, normal);
}

vec3 GetPhongBRDFDirectional(Light light, vec3 Cs, vec3 N, vec3 V, float n, vec3 part1)
{
	float intensity = light.values.x;

	vec3 L = light.direction;
	vec3 R = reflect(L, N);
	vec3 Li = light.lightColor * intensity;
	float lambert = max(dot(N, -L), 0.0);
	vec3 Rf = Cs + (1.0 - Cs) * pow(1.0 - lambert, 5.0);
	
	return (part1 + ((n + 2.0)/2.0) * Rf * pow(max(dot(V, R), 0.0001), n)) * Li * lambert;
}

vec3 GetPhongBRDFPoint(Light light, vec3 Cs, vec3 N, vec3 V, float n, vec3 pos, vec3 part1) 
{
	float intensity = light.values.x;
	float radius = light.values.y;

	float dist = distance(light.position, pos);
	float Fatt = pow(max(1 - pow(dist / radius, 4.0), 0.0), 2.0)/(dist * dist + 1);
	vec3 L = normalize(pos - light.position);
	vec3 R = reflect(L, N);
	vec3 Li = light.lightColor * intensity * Fatt;
	float lambert = max(-dot(N, L), 0.0);
	vec3 Rf = Cs + (1.0 - Cs) * pow(1.0 - lambert, 5.0);
	
	return (part1 + ((n + 2.0)/2.0) * Rf * pow(max(dot(V, R), 0.0001), n)) * Li * lambert;
}

vec3 GetPhongBRDFSpot(Light light, vec3 Cs, vec3 N, vec3 V, float n, vec3 pos, vec3 part1) 
{
	float intensity = light.values.x;
	float radius = light.values.y;
	float innerAngle = light.values.z;
	float outerAngle = light.values.w;

	vec3 L = normalize(pos - light.position);
	float C = dot(L, light.direction);
	float Catt = 0;
	if (C >= cos(innerAngle))
		Catt = 1;
	else if (C < cos(innerAngle) && C >= cos(outerAngle))
		Catt = (C - cos(outerAngle))/(cos(innerAngle) - cos(outerAngle));
		
	float dist = distance(light.position, pos);
	float Fatt = pow(max(1 - pow(dist / radius, 4.0), 0.0), 2.0)/(dist * dist + 1);
	vec3 R = reflect(L, N);
	vec3 Li = light.lightColor * intensity * Fatt * Catt;
	float lambert = max(-dot(N, L), 0.0);
	vec3 Rf = Cs + (1.0 - Cs) * pow(1.0 - lambert, 5.0);
	
	return (part1 + ((n + 2.0)/2.0) * Rf * pow(max(dot(V, R), 0.0001), n)) * Li * lambert;
}

void main()
{    
	mat3 tbn = CreateTangentSpace(normalize(normal), normalize(tangent));
	vec3 fragmentNormal = tbn * (texture(normalMap, uv0).xyz * 2.0 - 1.0);

    vec3 N = normalize(fragmentNormal);
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
 	
 	vec3 finalColor = vec3(0.0, 0.0, 0.0);
    for (int i = 0; i < min(numLights, MAX_NUM_TOTAL_LIGHTS); ++i) 
    {
		if (lights[i].lightType == 0)
			finalColor += GetPhongBRDFDirectional(lights[i], Cs, N, V, n, part1);
		else if (lights[i].lightType == 1)
    		finalColor += GetPhongBRDFPoint(lights[i], Cs, N, V, n, position, part1);
		else if (lights[i].lightType == 2)
    		finalColor += GetPhongBRDFSpot(lights[i], Cs, N, V, n, position, part1);
    }
    
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

#version 330

in vec2 uv0;
in vec3 normal;
in vec3 position; 

uniform vec3 cameraPosition; 
uniform sampler2D texture;
uniform vec3 lightDirection;
uniform vec3 ambientColor;
uniform vec3 specularColor;
uniform float shininess;
uniform vec3 lightColor;

struct Material
{
    vec3 diffuse_color;
    sampler2D diffuse_map;
    vec3 specular_color;
    sampler2D specular_map;
    float shininess;
    vec3 ambient;

    int has_diffuse_map;
    int has_specular_map;
    int shininess_alpha;
};

out vec4 outColor;

vec3 phongBRDF(vec3 lightDir, vec3 viewDir, vec3 normal, vec3 phongDiffuseCol, vec3 phongSpecularCol, float phongShininess) 
{
    vec3 color = phongDiffuseCol;
    vec3 reflectDir = reflect(-lightDir, normal);
    float specDot = max(dot(reflectDir, viewDir), 0.0);
    color += pow(specDot, phongShininess) * phongSpecularCol;
    return color;
}

void main()
{
    //vec3 diffuseColor = vec3(texture2D(texture, uv0));
    vec3 diffuseColor = vec3(pow(texture2D(texture, uv0).r, 2.2), pow(texture2D(texture, uv0).g, 2.2), pow(texture2D(texture, uv0).b, 2.2));

    vec3 lightDir = normalize(-lightDirection);
    vec3 viewDir = normalize(cameraPosition - position);
    vec3 n = normalize(normal);

    vec3 luminance = ambientColor.rgb;
  
    float illuminance = dot(lightDir, n);
    if(illuminance > 0.0) 
    {
        //shininess = exp2(alpha*7+1);
        vec3 brdf = phongBRDF(lightDir, viewDir, n, diffuseColor, specularColor, shininess);
        luminance += brdf * illuminance * lightColor;
    }

    outColor.rgb = luminance;
    //outColor.rgb = pow(outColor.rgb, vec3(1/2.2));
    outColor.a = 1.0;
}

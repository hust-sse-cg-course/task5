#version 330 core

struct Material {
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};

// 定向光源
struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in vec4 LightSpaceFragPos;

out vec4 FragColor;

uniform vec3 viewPos;
uniform Material material;
uniform DirLight dirLight;
uniform sampler2D depthMap;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir,float shadow)
{
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 diffuseColor = vec3(texture(material.diffuse, TexCoords));
    vec3 ambient = light.ambient * diffuseColor;
    vec3 diffuse = light.diffuse * diff * diffuseColor;
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    return ambient +  (1.0f-shadow)*(diffuse + specular);
}

float ShadowCalculation(vec4 fragPosLightSpace)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(depthMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
	float bias = 0.005;
	//float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;
    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    return shadow;
}

void main()
{
    vec3 norm = normalize(Normal);
	if(!gl_FrontFacing) norm = -norm;
    vec3 viewDir = normalize(viewPos - FragPos);
	float shadow = ShadowCalculation(LightSpaceFragPos);
    vec3 result = CalcDirLight(dirLight, norm, viewDir,shadow);
    FragColor = vec4(result, 1.0);
}
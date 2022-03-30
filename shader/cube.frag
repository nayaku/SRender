#version 330 core
struct Material
{
    sampler2D diffuse;
    sampler2D specular;
    float shiniess;
};
struct Light
{
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 viewPos;
uniform Material material;
uniform Light light;

out vec4 FragColor;

void main()
{
    vec3 lightDir = normalize(light.position - FragPos);
    float theta = dot(lightDir, normalize(-light.direction));

    if(theta > light.cutOff)
    {
        // 环境光
        vec3 ambient = light.ambient * vec3(texture(material.diffuse,TexCoords));
    
        // 漫反射
        vec3 norm = normalize(Normal);
        float diff = max(dot(norm,lightDir),0.0);
        vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse,TexCoords));

        // 镜面反射
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir,norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0),material.shiniess);
        vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
        
        // 衰减
        float distance = length(light.position - FragPos);
        float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

        diffuse *= attenuation;
        specular *= attenuation;

        vec3 result = ambient + diffuse + specular;
        FragColor = vec4(result, 1.0);
    }
    else
    {
        FragColor = vec4(light.ambient  * vec3(texture(material.diffuse,TexCoords)),1.0);
    }
}
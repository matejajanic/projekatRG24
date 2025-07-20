//#shader vertex
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;
out vec3 Normal;
out vec3 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoords = aTexCoords;

    gl_Position = projection * view * vec4(FragPos, 1.0);
}

//#shader fragment
#version 330 core

out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

uniform vec3 viewPos;
uniform sampler2D texture_diffuse1;
uniform float shininess;

struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform DirLight dir_light;

#define NR_POINT_LIGHTS 5

struct PointLight {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float constant;
    float linear;
    float quadratic;
};

uniform PointLight pointLights[NR_POINT_LIGHTS];

vec3 calculate_dir_light(DirLight d_light, vec3 normal, vec3 view_dir);
vec3 calculate_point_light(PointLight point_light, vec3 normal, vec3 fragPos, vec3 viewDir);


void main()
{
    vec3 norm = normalize(Normal);
    vec3 view_dir = normalize(viewPos - FragPos);

    vec3 res = calculate_dir_light(dir_light, norm, view_dir);
    for (int i = 0; i < NR_POINT_LIGHTS; i++) {
        res += calculate_point_light(pointLights[i], norm, FragPos, view_dir);
    }

    FragColor = vec4(res, 1.0);
}

vec3 calculate_dir_light(DirLight d_light, vec3 normal, vec3 view_dir) {
    vec3 light_dir = normalize(-d_light.direction);
    //Ambient
    vec3 texColor = texture(texture_diffuse1, TexCoords).rgb;
    vec3 ambient = d_light.ambient * texColor;

    // Diffuse
    float diff = max(dot(normal, light_dir), 0.0);
    vec3 diffuse = d_light.diffuse * diff * texColor;

    // Specular
    vec3 reflectDir = reflect(-light_dir, normal);
    float spec = pow(max(dot(view_dir, reflectDir), 0.0), shininess);
    vec3 specular = dir_light.specular * spec * vec3(1.0);

    return ambient + diffuse + specular;
}

vec3 calculate_point_light(PointLight point_light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 light_dir = normalize(point_light.position - fragPos);
    float diff = max(dot(normal, light_dir), 0.0);

    vec3 reflect_dir = reflect(-light_dir, normal);
    float spec = pow(max(dot(viewDir, reflect_dir), 0.0), shininess);

    float distance = length(point_light.position - fragPos);
    float attenuation = 1.0 / (point_light.constant + point_light.linear * distance + point_light.quadratic * (distance * distance));

    vec3 texColor = texture(texture_diffuse1, TexCoords).rgb;

    vec3 ambient = point_light.ambient * texColor * 1.2;
    vec3 diffuse = point_light.diffuse * diff * texColor * 1.5;
    vec3 specular = point_light.specular * spec * vec3(1.0) * 1.5;

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return ambient + diffuse + specular;
}
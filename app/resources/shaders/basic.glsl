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

void main() {
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = aNormal;
    TexCoords = aTexCoords;
    gl_Position = projection * view * vec4(FragPos, 1.0);
}

//#shader fragment
#version 330 core

out vec4 FragColor;

struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 specular;
    vec3 diffuse;
};

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

uniform vec3 viewPos;
uniform float shininess;
uniform sampler2D texture_diffuse1;

uniform DirLight dir_light;

vec3 calculate_dir_light(DirLight light, vec3 normal, vec3 view_dir);

void main() {
    vec3 norm = normalize(Normal);
    vec3 view_dir = normalize(viewPos - FragPos);

    vec3 res = calculate_dir_light(dir_light, norm, view_dir);
    FragColor = vec4(texture(texture_diffuse1, TexCoords).rgb, 1.0);
}

vec3 calculate_dir_light(DirLight light, vec3 normal, vec3 view_dir) {
    vec3 light_dir = normalize(-light.direction);
    float diff = max(dot(normal, light_dir), 0.0);
    vec3 reflect_dir = reflect(-light_dir, normal);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), shininess);

    vec3 texColor = texture(texture_diffuse1, TexCoords).rgb;

    vec3 ambient = light.ambient * texColor;
    vec3 diffuse = light.diffuse * diff * texColor;
    vec3 specular = light.specular * spec * vec3(1.0);

    return ambient + diffuse + specular;
}
//#shader vertex
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

void main()
{
    TexCoords = aTexCoords;
    gl_Position = vec4(aPos, 0.0, 1.0);
}

//#shader fragment
#version 330 core
in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D uImage;
uniform bool uHorizontal;

const float weights[5] = float[](0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main()
{
    vec2 texel = 1.0 / vec2(textureSize(uImage, 0));
    vec3 result = texture(uImage, TexCoords).rgb * weights[0];

    for (int i = 1; i < 5; ++i) {
        vec2 off = (uHorizontal ? vec2(texel.x * i, 0.0) : vec2(0.0, texel.y * i));
        result += texture(uImage, TexCoords + off).rgb * weights[i];
        result += texture(uImage, TexCoords - off).rgb * weights[i];
    }
    FragColor = vec4(result, 1.0);
}

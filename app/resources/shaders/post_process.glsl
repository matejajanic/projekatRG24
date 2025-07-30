//#shader vertex
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;
out vec2 TexCoords;

void main() {
    TexCoords = aTexCoords;
    gl_Position = vec4(aPos, 0.0, 1.0);
}

//#shader fragment
#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform bool sharpen;
uniform vec2 offset;

void main() {
    if (!sharpen) {
        FragColor = texture(screenTexture, TexCoords);
    } else {
        vec2 offsets[9] = vec2[](
        vec2(-offset.x, offset.y),
        vec2(0.0, offset.y),
        vec2(offset.x, offset.y),
        vec2(-offset.x, 0.0),
        vec2(0.0, 0.0),
        vec2(offset.x, 0.0),
        vec2(-offset.x, -offset.y),
        vec2(0.0, -offset.y),
        vec2(offset.x, -offset.y)
        );
        float kernel[9] = float[](
        0, -1, 0,
        -1, 5, -1,
        0, -1, 0
        );
        vec3 sampleTex[9];
        for (int i = 0; i < 9; i++)
        sampleTex[i] = vec3(texture(screenTexture, TexCoords + offsets[i]));

        vec3 col = vec3(0.0);
        for (int i = 0; i < 9; i++)
        col += sampleTex[i] * kernel[i];

        FragColor = vec4(col, 1.0);
    }
}

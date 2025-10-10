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

uniform sampler2D uScene;
uniform sampler2D uBloom;
uniform float uStrength;
uniform bool uSharpen;
uniform vec2 uOffset;

vec3 sharpen(vec2 uv) {
    vec2 off = uOffset;
    vec3 c = texture(uScene, uv).rgb;
    vec3 s = -texture(uScene, uv + vec2(-off.x, -off.y)).rgb
    - texture(uScene, uv + vec2(0.0f, -off.y)).rgb
    - texture(uScene, uv + vec2(off.x, -off.y)).rgb
    - texture(uScene, uv + vec2(-off.x, 0.0f)).rgb
    + c * 9.0
    - texture(uScene, uv + vec2(off.x, 0.0f)).rgb
    - texture(uScene, uv + vec2(-off.x, off.y)).rgb
    - texture(uScene, uv + vec2(0.0f, off.y)).rgb
    - texture(uScene, uv + vec2(off.x, off.y)).rgb;
    return s;
}

void main()
{
    vec3 scene = texture(uScene, TexCoords).rgb;
    vec3 bloom = texture(uBloom, TexCoords).rgb;

    if (uSharpen)
    scene = sharpen(TexCoords);

    vec3 color = scene + bloom * uStrength;
    FragColor = vec4(clamp(color, 0.0, 1.0), 1.0);
}

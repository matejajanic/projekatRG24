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
uniform float uTargetHue;  // ~0.50 (cijan/tirkiz)
uniform float uHueWidth;   // ~0.08
uniform float uMinS;       // ~0.25
uniform float uMinV;       // ~0.60

// RGB → HSV
vec3 rgb2hsv(vec3 c) {
    float M = max(max(c.r, c.g), c.b);
    float m = min(min(c.r, c.g), c.b);
    float d = M - m;
    float h = 0.0;
    if (d > 1e-5) {
        if (M == c.r)      h = mod((c.g - c.b) / d, 6.0);
        else if (M == c.g) h = ((c.b - c.r) / d) + 2.0;
        else h = ((c.r - c.g) / d) + 4.0;
        h /= 6.0;
        if (h < 0.0) h += 1.0;
    }
    float s = (M <= 0.0) ? 0.0 : d / M;
    float v = M;
    return vec3(h, s, v);
}

float hueDistance(float h1, float h2) {
    float d = abs(h1 - h2);
    return min(d, 1.0 - d);
}

void main()
{
    vec3 col = texture(uScene, TexCoords).rgb;
    vec3 hsv = rgb2hsv(col);

    float d = hueDistance(hsv.x, uTargetHue);

    float passHue = smoothstep(uHueWidth, uHueWidth * 0.7, d);
    float passSat = smoothstep(uMinS, uMinS * 1.2, hsv.y);
    float passVal = smoothstep(uMinV, uMinV * 1.2, hsv.z);

    float mask = passHue * passSat * passVal;

    FragColor = vec4(col * mask, 1.0);
}

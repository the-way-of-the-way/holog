#include "Uniforms.glsl"
#include "Samplers.glsl"
#include "Transform.glsl"

const float PI = 3.14159265;

varying vec2 vTexCoord;

bool isInTriangle(vec2 p0, vec2 p1, vec2 p2, vec2 p) {
    float a = 1. / 2. * (-p1.y * p2.x + p0.y * (-p1.x + p2.x) + p0.x * (p1.y - p2.y) + p1.x * p2.y);
    float s = 1. / (2. * a) * (p0.y * p2.x - p0.x * p2.y + (p2.y - p0.y) * p.x + (p0.x - p2.x) * p.y);
    float t = 1. / (2. * a) * (p0.x * p1.y - p0.y * p1.x + (p0.y - p1.y) * p.x + (p1.x - p0.x) * p.y);
    // >= Fixes black lines
    return (s >= 0.) && (t >= 0.) && (1. - s - t >= 0.);
}

vec2 rotate(vec2 v, float a) {
    vec2 sv = v - vec2(.5);
    vec2 rv = vec2(sv.x * cos(a) - sv.y * sin(a), sv.x * sin(a) + sv.y * cos(a));
    vec2 av = rv + vec2(.5);
    return av;
}

void VS() {
    mat4 modelMatrix = iModelMatrix;
    vec3 worldPos = GetWorldPos(modelMatrix);
    gl_Position = GetClipPos(worldPos);
    vTexCoord = iTexCoord;
}

void PS() {
    vec2 p = vTexCoord;
    float center = .5;
    float offset = .2;
    if (isInTriangle(vec2(0.), vec2(1., 0.), vec2(center), p)) {
        // Bottom
        gl_FragColor = texture2D(sDiffMap, vec2(vTexCoord.x, vTexCoord.y + offset));
    } else if (isInTriangle(vec2(0., 1.), vec2(0.), vec2(center), p)) {
        // Left
        vec2 rotated = rotate(vTexCoord, 3. * PI / 2.);
        gl_FragColor = texture2D(sNormalMap, vec2(rotated.x, 1. - rotated.y + offset));
    } else if (isInTriangle(vec2(0., 1.), vec2(center), vec2(1.), p)) {
        // Top
        gl_FragColor = texture2D(sSpecMap, vec2(vTexCoord.x, 1. - vTexCoord.y + offset));
    } else if (isInTriangle(vec2(1.), vec2(center), vec2(1., 0.), p)) {
        // Right
        vec2 rotated = rotate(vTexCoord, PI / 2.);
        gl_FragColor = texture2D(sEmissiveMap, vec2(rotated.x, 1. - rotated.y + offset));
    } else {
        gl_FragColor = vec4(0.0);
    }
}

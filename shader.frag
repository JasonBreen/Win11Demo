// Tunnel effect starter for Shadertoy-style demo
#version 120
uniform float iTime;
uniform vec2 iResolution;
uniform vec4 iMouse;

void main() {
    vec2 uv = (gl_FragCoord.xy / iResolution.xy) * 2.0 - 1.0;
    float angle = atan(uv.y, uv.x);
    float radius = length(uv);
    float stripes = sin(angle * 10.0 + iTime * 2.0);
    float tunnel = 0.5 + 0.5 * cos(radius * 8.0 - iTime * 3.0);
    float color = stripes * tunnel;
    gl_FragColor = vec4(0.5 + 0.5 * cos(iTime + color * 2.0), 0.5 + 0.5 * sin(iTime + color * 2.0), color, 1.0);
}

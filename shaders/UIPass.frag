#version 450

layout(location = 0) in vec2 inUV;

layout(location = 0) out vec4 outColor;

void main() 
{
    // conver uv to color 
    vec3 color = vec3(inUV, 0.0);
    outColor = vec4(color, 1.0);
}

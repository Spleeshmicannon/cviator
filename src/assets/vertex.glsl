#version 430 core
layout(location = 0) in vec4 aPos;
layout(location = 1) in vec3 aColor;
layout(location = 2) in float aMass;

out vec3 geoColor;
out float mass;

void main()
{
    gl_Position = vec4(aPos.x, aPos.y, 0f, 1.0f);
    geoColor = vec3(aColor);
    mass = aMass;
}

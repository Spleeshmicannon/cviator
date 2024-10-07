#version 430 core
layout(local_size_x = 1000) in;

layout(std430, binding = 0) buffer Data {
    vec4 vertices[];
};

layout(std430, binding = 1) readonly buffer readData {
    vec4 readv[];
};

void main()
{
    const uint idx = gl_GlobalInvocationID.x;

    float fx = 0;
    float fy = 0;

    const float zero = 0.000000000000000000000000000000000000001f;
    const float mass = 0.01f;
    const int size = 50000;

    for(int i = 0; i < size; ++i)
    {
        if(i == idx) continue;
        float F, rx, ry;

        rx = readv[idx].x - readv[i].x;
        ry = readv[idx].y - readv[i].y;
        
        const float G = 0.0000006743;
        F = (mass * mass * G) / 
            (((rx * rx) + (ry * ry)) + 
            zero); // stopping divide by 0

        fx += (-F * rx) + zero;
        fy += (-F * ry) + zero;
    }

    // z (dx) and w (dy)
    vertices[idx].z += (fx / mass);
    vertices[idx].w += (fy / mass);

    vertices[idx].x += vertices[idx].z;
    vertices[idx].y += vertices[idx].w;
}

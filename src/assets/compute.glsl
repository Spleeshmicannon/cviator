#version 430 core
layout(local_size_x = 1000) in;

struct data {
    vec4 vertices;
    vec3 color;
    float mass;
};

layout(std430, binding = 0) buffer Data {
    data write[];    
};

layout(std430, binding = 1) readonly buffer readData {
    data read[];
};

void main()
{
    const uint idx = gl_GlobalInvocationID.x;

    float fx = 0;
    float fy = 0;

    const float zero = 0.000000000000000000000000000000000000001f;
    const float mass = read[idx].mass;
    const int size = 50000;

    for(int i = 0; i < size; ++i)
    {
        if(i == idx) continue;
        float F, rx, ry;

        rx = read[idx].vertices.x - read[i].vertices.x;
        ry = read[idx].vertices.y - read[i].vertices.y;
        
        const float G = 0.0000006743;
        F = (mass * mass * G) / 
            (((rx * rx) + (ry * ry)) + 
            zero); // stopping divide by 0

        fx += (-F * rx) + zero;
        fy += (-F * ry) + zero;
    }

    // z (dx) and w (dy)
    write[idx].vertices.z += (fx / mass);
    write[idx].vertices.w += (fy / mass);

    write[idx].vertices.x += write[idx].vertices.z;
    write[idx].vertices.y += write[idx].vertices.w;

    // set the colour based on velocity
    // the length refers to the length (magnitude) of the velocity vector
    float velocityMagnitude = length(vec2(read[idx].vertices.z, read[idx].vertices.w));
    
    // normalize
    float normalizedVelocity = clamp(velocityMagnitude / 0.015, 0.0, 1.0);
    
    // map to a colour
    write[idx].color = mix(vec3(0.0, 0.0, 1.0), vec3(1.0, 0.0, 0.0), normalizedVelocity);
}

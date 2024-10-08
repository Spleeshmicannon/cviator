#version 430 core

layout(points) in;
layout(triangle_strip, max_vertices = 6) out;

out vec2 texCoord;

void main() {
    const float pointSize = 0.005;

    vec4 center = gl_in[0].gl_Position;

    // First triangle
    texCoord = vec2(0, 0);
    gl_Position = center + vec4(-pointSize, -pointSize, 0.0, 0.0); // Bottom-left
    EmitVertex();
    
    texCoord = vec2(1, 0);
    gl_Position = center + vec4(pointSize, -pointSize, 0.0, 0.0);  // Bottom-right
    EmitVertex();
    
    texCoord = vec2(0, 1);
    gl_Position = center + vec4(-pointSize, pointSize, 0.0, 0.0);  // Top-left
    EmitVertex();
    EndPrimitive();

    // Second triangle
    texCoord = vec2(1, 0);
    gl_Position = center + vec4(pointSize, -pointSize, 0.0, 0.0);  // Bottom-right
    EmitVertex();
    
    texCoord = vec2(1, 1);
    gl_Position = center + vec4(pointSize, pointSize, 0.0, 0.0);   // Top-right
    EmitVertex();
    
    texCoord = vec2(0, 1);
    gl_Position = center + vec4(-pointSize, pointSize, 0.0, 0.0);  // Top-left (repeated)
    EmitVertex();
    EndPrimitive();
}

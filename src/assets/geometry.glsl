#version 430 core
layout(points) in;

layout(triangle_strip, max_vertices = 6) out;

in vec3 geoColor[];
in float mass[];

out vec2 texCoord;
out vec3 fragColor;

void main() {

    const vec4 center = gl_in[0].gl_Position;
    const float pointSize = mass[0];
    const vec3 outColor = geoColor[0];

    // First triangle
    texCoord = vec2(0, 0);
    fragColor = vec3(outColor.x, outColor.y, outColor.z);
    gl_Position = center + vec4(-pointSize, -pointSize, 0.0, 0.0); // Bottom-left
    EmitVertex();
    
    texCoord = vec2(1, 0);
    fragColor = vec3(outColor.x, outColor.y, outColor.z);
    gl_Position = center + vec4(pointSize, -pointSize, 0.0, 0.0);  // Bottom-right
    EmitVertex();
    
    texCoord = vec2(0, 1);
    fragColor = vec3(outColor.x, outColor.y, outColor.z);
    gl_Position = center + vec4(-pointSize, pointSize, 0.0, 0.0);  // Top-left
    EmitVertex();
    EndPrimitive();

    // Second triangle
    texCoord = vec2(1, 0);
    fragColor = vec3(outColor.x, outColor.y, outColor.z);
    gl_Position = center + vec4(pointSize, -pointSize, 0.0, 0.0);  // Bottom-right
    EmitVertex();
    
    texCoord = vec2(1, 1);
    fragColor = vec3(outColor.x, outColor.y, outColor.z);
    gl_Position = center + vec4(pointSize, pointSize, 0.0, 0.0);   // Top-right
    EmitVertex();
    
    texCoord = vec2(0, 1);
    fragColor = vec3(outColor.x, outColor.y, outColor.z);
    gl_Position = center + vec4(-pointSize, pointSize, 0.0, 0.0);  // Top-left (repeated)
    EmitVertex();
    EndPrimitive();
}

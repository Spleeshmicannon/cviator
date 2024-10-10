#version 430 core
out vec4 FragColor;
in vec2 texCoord;
in vec3 fragColor;

uniform sampler2D texture_data;

void main()
{
    FragColor = texture(texture_data, texCoord) * vec4(fragColor.x, fragColor.y, fragColor.z, 1);
}

#version 430 core
out vec4 FragColor;
in vec2 texCoord;

uniform sampler2D texture_data;

void main()
{
    FragColor = texture(texture_data, texCoord);
}

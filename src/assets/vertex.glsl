#version 430 core
layout(location = 0) in vec4 aPos;

void main()
{
    gl_Position = vec4(aPos.x , aPos.y, 0f, 1.0f);
}

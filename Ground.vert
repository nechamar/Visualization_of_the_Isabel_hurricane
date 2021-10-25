#version 330

in vec4 color;
in vec3 position;

uniform mat4 PVMmatrix;

out vec4 color_v;

void main()
{
    color_v = color;
    gl_Position = PVMmatrix * vec4(position, 1);
}

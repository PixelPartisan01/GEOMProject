#version 460

in vec3 attrib_position;

void main()
{
    gl_Position = vec4(attrib_position, 1.0);
}
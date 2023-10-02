#version 460

layout (location = 0) in vec3 position;
//layout (location = 1) in vec3 color;

uniform mat4 proj, view, model, vert;
uniform mat4 vert_fl;
uniform int sw;
out vec3 vertexColor;

void main()
{
	gl_Position = proj * view * model * vert * vec4(position, 1.0);
	vertexColor = vec3(0.0f, 1.0f, 1.0f);
};

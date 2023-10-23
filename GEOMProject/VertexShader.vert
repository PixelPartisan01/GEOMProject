#version 460

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 position_Bez;
layout (location = 2) in vec3 position_con_mesh;
layout (location = 3) in vec3 position_Bez_surface;

uniform mat4 proj, view, model, vert;
uniform int surface;
out vec3 vertexColor;

void main()
{
	if (surface == 0)
	{
		gl_Position = proj * view * model * vert * vec4(position, 1.0);
		vertexColor = vec3(0.0f, 1.0f, 1.0f);
	}
	else if (surface == 1)
	{
		gl_Position = proj * view * model * vert * vec4(position_Bez, 1.0);
		vertexColor = vec3(1.0f, 0.0f, 1.0f);
	}
	else if (surface == 2)
	{
		gl_Position = proj * view * model * vert * vec4(position_con_mesh, 1.0);
		vertexColor = vec3(0.0f, 1.0f, 1.0f);
	}
	else if (surface == 3)
	{
		gl_Position = proj * view * model * vert * vec4(position_Bez_surface, 1.0);
		vertexColor = vec3(1.0f, 1.0f, 0.0f);
	}
	
};

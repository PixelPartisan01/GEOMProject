#version 460

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 position_Bez;
layout (location = 2) in vec3 position_con_mesh;
layout (location = 3) in vec3 position_Bez_surface;
layout (location = 4) in vec3 norm_Bez_surface;
layout (location = 5) in vec3 light_pos;

uniform mat4 proj, view, model, vert;
uniform int surface;
out vec4 vertexColor;

//vec3  ambient	= vec3(1.0, 0.0, 0.0);
float shine		= 100.0;

void main()
{
	if (surface == 0)
	{
		gl_Position = proj * view * model * vert * vec4(position, 1.0);
		vertexColor = vec4(0.0f, 1.0f, 1.0f, 1.0f);
	}
	else if (surface == 1)
	{
		gl_Position = proj * view * model * vert * vec4(position_Bez, 1.0);
		vertexColor = vec4(1.0f, 0.0f, 1.0f, 1.0f);
	}
	else if (surface == 2)
	{
		gl_Position = proj * view * model * vert * vec4(position_con_mesh, 1.0);
		vertexColor = vec4(0.0f, 1.0f, 1.0f, 1.0f);
	}
	else if (surface == 3)
	{
		vec4 ambient = vec4(position_Bez_surface, 1);
		vec4 diffuse = vec4(dot(light_pos, norm_Bez_surface) * vec3(1.0, 1.0, 1.0), 1);

		gl_Position = proj * view * model * vert * vec4(position_Bez_surface, 1.0);
		vertexColor = ambient + diffuse + vec4(1.0f, 1.0f, 0.0f, 1.0f);
	}
	
};

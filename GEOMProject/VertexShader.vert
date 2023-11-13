#version 460

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 position_Bez;
layout (location = 2) in vec3 position_con_mesh;
layout (location = 3) in vec3 position_Bez_surface;
layout (location = 4) in vec3 norm_Bez_surface;

uniform mat4	proj, view, model, vert;
uniform int		surface;
uniform vec3 light_pos;
uniform int shading;
uniform bool selected_cont_p;

in vec4 a_Position;
in vec3 a_Normal; 


out vec3 fragColor;
out vec4 Normal;
out vec4 FragPos;
out int  control_mesh;
//out vec3 modelViewVertex, modelViewNormal, lightVector;
out float diffuse_gouraud;


void main()
{
	if (surface == 0)
	{
		control_mesh = 1;
		gl_Position = proj * view * model * vert * vec4(position, 1.0);

		if(selected_cont_p){fragColor = vec3(1.0f, 0.0f, 0.0f);}
		else{fragColor = vec3(229.0f / 255.0f, 2.0f / 255.0f, 245.0f / 255.0f);}
	}
	else if (surface == 1)
	{
		gl_Position = proj * view * model * vert * vec4(position_Bez, 1.0);
		fragColor = vec3(19.0f / 255.0f, 245.0f / 255.0f , 2.0f / 255.0f);
	}
	else if (surface == 2)
	{
		control_mesh = 1;
		gl_Position = proj * view * model * vert * vec4(position_con_mesh, 1.0);
		fragColor = vec3(245.0f / 255.0f, 115.0f / 255.0f, 2.0f / 255.0f);
	}
	else if (surface == 3)
	{
		control_mesh = 0;
		vec3 ne = norm_Bez_surface / normalize(norm_Bez_surface);
		gl_Position = proj * view * model * vert * vec4(position_Bez_surface, 1.0);

		vec3 ambient = 1.0 * vec3(1.0, 1.0, 1.0);
//
		vec3 modelViewVertex = vec3(view * model * vec4(model * vec4(position_Bez_surface, 1.0)));
		vec3 modelViewNormal = vec3(view * model * vec4(norm_Bez_surface, 0.0));
		float dist = length(light_pos - modelViewVertex);
		vec3 lightVector = normalize(light_pos - modelViewVertex);
		diffuse_gouraud = max(dot(modelViewNormal, lightVector), 0.1) * (1.0 / (1.0 + (0.25 * dist * dist)));

		fragColor = vec3(19.0f / 255.0f, 245.0f / 255.0f , 2.0f / 255.0f) * ambient;
		FragPos = vec4(model * vec4(position_Bez_surface, 1.0));
		Normal = vec4(norm_Bez_surface, 1.0);
	}
}

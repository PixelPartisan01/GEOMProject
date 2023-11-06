#version 460

in vec3 fragColor;
in vec4 Normal;
in vec4 FragPos;
in vec4 lp;
flat in int control_mesh;

//uniform vec3 light_pos;
uniform mat4 model;
uniform mat4 view;

out vec4 color;

void main()
{
	vec4 lightColor = vec4(1.0, 1.0, 1.0, 1.0);
	vec4 norm = normalize(Normal);
//	vec4 lightDir = normalize(lp - FragPos);
//	float diff = max(dot(norm, lightDir), 0.0);
//	vec4 diffuse = diff * lightColor;
	float cos_angle = dot(norm, lp);
	cos_angle = clamp(cos_angle, 0.0, 1.0);
	vec4 diff;
	if(control_mesh == 0)
	{
		diff = cos_angle * lightColor;
	}
	else
	{
		diff = vec4(1.0, 1.0, 1.0, 1.0);
	}

	color = vec4(fragColor,1.0) * diff;
};
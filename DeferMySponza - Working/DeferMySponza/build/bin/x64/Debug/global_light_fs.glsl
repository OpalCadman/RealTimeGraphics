#version 330

struct PerDirectionalUniform
{
	vec3 direction;
	float padding;
	vec3 intensity;
	float padding2;
};

layout(std140) uniform global_lights
{
	vec3 Ambient_Intensity;
	int directional_light_count;
	PerDirectionalUniform directional_lights[2];
};

uniform sampler2DRect sampler_world_position;
uniform sampler2DRect sampler_world_normal;


out vec4 fragment_colour;

void main(void)
{
	vec3 texel_N = texture(sampler_world_normal, gl_FragCoord.xy).rgb;
	vec3 N = normalize(texel_N);
	vec3 diffuse = Ambient_Intensity;

	for (int i = 0; i < directional_light_count; i++)
	{
		float a = max(0, dot(N, directional_lights[i].direction));

		diffuse += directional_lights[i].intensity * a;
	}

	fragment_colour = vec4(diffuse, 1.0);
}

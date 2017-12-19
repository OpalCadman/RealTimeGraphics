#version 330

layout(std140) uniform point_lights
{
	vec3 Intensity;
	float range;
	vec3 Position;
	float cone_angle;
	vec3 Direction;
	float padding;
	mat4 light_model_xform;
};

uniform sampler2DRect sampler_world_position;
uniform sampler2DRect sampler_world_normal;

out vec4 fragment_colour;

void main(void)
{
	vec3 N = texture(sampler_world_normal, gl_FragCoord.xy).xyz;
	vec3 L = Position - texture(sampler_world_position, gl_FragCoord.xy).xyz;

	float distance = length(L);
	float attenuation = (1.0 - smoothstep(0.0, range, distance));
	float NdotL = max(dot(N, normalize(L)), 0.0);

	// Do the angles.
	float angleAttenuation = max(0.0, dot(Direction, normalize(-L)));
	if (angleAttenuation > cos(cone_angle))
	{
		angleAttenuation = 1.0;
	}
	else
	{
		angleAttenuation = 0.0;
	}

	fragment_colour = vec4(Intensity * NdotL * attenuation * angleAttenuation, 1.0);
	//fragment_colour = vec4(1.0, 0.0, 1.0, 1.0);
}

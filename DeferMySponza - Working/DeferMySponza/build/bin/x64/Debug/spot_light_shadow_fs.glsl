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
uniform sampler2D sampler_shadow_texture;
uniform mat4 shadow_vp;

out vec4 fragment_colour;

void main(void)
{
	vec3 N = texture(sampler_world_normal, gl_FragCoord.xy).xyz;
	vec3 P = texture(sampler_world_position, gl_FragCoord.xy).xyz;
	vec3 L = Position - P;

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

	float visibility = 1.0;

	vec4 lightpos = shadow_vp * vec4(P, 1);
	lightpos.xyz = lightpos.xyz / lightpos.w;
	lightpos.xyz = lightpos.xyz / 2 + 0.5;
	float texDepth = texture(sampler_shadow_texture, lightpos.x);
	if (texDepth < lightpos.z)
	{
		visibility = 0.0;
	}

	fragment_colour = vec4(Intensity * NdotL * attenuation * angleAttenuation, 1.0) * visibility;
}

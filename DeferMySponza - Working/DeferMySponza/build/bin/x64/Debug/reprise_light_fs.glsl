#version 330

layout(std140) uniform point_lights
{
	vec3 Intensity;
	float range;
	vec3 Position;
	float padding;
};

uniform sampler2DRect sampler_world_position;
uniform sampler2DRect sampler_world_normal;


out vec4 fragment_colour;


void main(void)
{
	vec3 N = texture(sampler_world_normal, gl_FragCoord.xy).xyz;
	vec3 L = Position - texture(sampler_world_position, gl_FragCoord.xy).xyz;

	float distance = length(L);
	float NdotL = max(dot(N, normalize(L)), 0.0);

	fragment_colour = vec4(1.0,0.0,1.0,1.0);
	
}

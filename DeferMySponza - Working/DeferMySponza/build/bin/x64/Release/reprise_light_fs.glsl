#version 330

layout(std140) uniform PerDirectionalUniform
{
	vec3 direction;
	float padding;
	vec3 intensity;
	float padding2;
};

out vec4 fragment_colour;

in vec3 world_normal;

void main(void)
{
	vec3 world_normals = normalize(world_normal);

	float a = max(0, dot(direction, world_normals));

	vec3 diffuse = intensity * a;

	fragment_colour = vec4(diffuse, 1.0);
	
}

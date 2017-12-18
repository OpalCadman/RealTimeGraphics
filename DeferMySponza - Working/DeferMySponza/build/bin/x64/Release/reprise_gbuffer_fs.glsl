#version 330

out vec3 Positions;
out vec3 Normals;

in vec3 world_normal;
in vec3 world_position;

void main(void)
{
	Positions = world_position;
	Normals = normalize(world_normal);
}

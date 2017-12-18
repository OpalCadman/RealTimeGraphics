#version 330

out vec4 fragment_colour;

in vec3 normal_col;

void main(void)
{
	fragment_colour = vec4(normal_col, 1.0);
}

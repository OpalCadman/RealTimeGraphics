#version 330

layout(std140) uniform point_lights
{
	vec3 Intensity;
	float range;
	vec3 Position;
	float padding;
	mat4 light_model_xform;
};

in vec3 vertex_position;

void main(void)
{
	gl_Position = light_model_xform * vec4(vertex_position, 1.0);
}

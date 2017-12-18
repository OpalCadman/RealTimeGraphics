#version 330

struct ModelData
{
	mat4 projection_view_model_xform;
	mat4 model_xform;
};

layout(std140) uniform PerModelUniform
{
	ModelData models[16];
};

layout(location = 0)in vec3 vertex_position;
layout(location = 1)in vec3 normal;


void main(void)
{
	//BIG BUNNY
	//mat4 combined_xform = models[gl_InstanceID].projection_view_model_xform * models[gl_InstanceID].model_xform;

	mat4 combined_xform = models[gl_InstanceID].projection_view_model_xform;
	gl_Position = combined_xform * vec4(vertex_position, 1.0);

}

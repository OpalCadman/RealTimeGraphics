#pragma once

#include <sponza/sponza_fwd.hpp>
#include <tygra/WindowViewDelegate.hpp>
#include <tgl/tgl.h>
#include <glm/glm.hpp>

#include <vector>
#include <memory>
#include <unordered_map>

class MyView : public tygra::WindowViewDelegate
{
public:

	MyView();

	~MyView();

	void createShader(GLuint &shader_, GLenum shaderType, std::string stringFromFile);

	void drawSponza(GLuint &program_);

	void createProgram(GLuint &program_name, GLuint &vs, GLuint &Fragemnet_shader);

	void createGbufferProgram(GLuint &program_name, GLuint &vs, GLuint &Fragment_shader);

	GLuint createUniformBuffer(size_t size, void* data);

	void createSponzaMesh();

	void createquadmesh();

	void createspheremesh();

	void createconemesh();

	void setScene(const sponza::Context * sponza);

private:

	void windowViewWillStart(tygra::Window * window) override;

	void windowViewDidReset(tygra::Window * window,
		int width,
		int height) override;

	void windowViewDidStop(tygra::Window * window) override;

	void windowViewRender(tygra::Window * window) override;

	const sponza::Context * scene_;

	// To Do: Change names to something smaller

	GLuint first_program_;

	GLuint global_light_program;

	GLuint gbuffer_program_;

	GLuint point_light_program_;

	GLuint spot_light_program_;

	GLuint shadow_program_;

	GLuint vertex_vbo_;

	GLuint element_vbo_;

	GLuint point_light_ubo;

	GLuint global_light_ubo;

	GLuint sponza_vs;

	GLuint sponza_fs;

	GLuint point_light_vs;

	GLuint point_light_fs;

	GLuint spot_light_fs;

	GLuint gbuffer_vs;

	GLuint gbuffer_fs;

	GLuint global_light_fs;

	GLuint global_light_vs;

	GLuint shadow_vs;

	GLuint shadow_fs;

	GLuint per_model_ubo;

	GLuint vao_;

	glm::mat4 view_projection;

	struct MeshGL
	{
		GLuint first_element_index;
		GLuint element_count;
		GLuint first_vertex_index;
	};


	struct Mesh
	{
		GLuint vertex_vbo{ 0 };
		GLuint element_vbo{ 0 };
		GLuint vao{ 0 };
		int element_count{ 0 };
	};


	struct PerModelUniforms
	{
		glm::mat4 projection_view_model_xform;
		glm::mat4 model_xform;
		
	};

	struct PerDirectionalUniform
	{
		glm::vec3 direction;
		float padding;
		glm::vec3 intensity;
		float padding2;
	};

	struct global_lights
	{
		glm::vec3 Ambient_Intensity;
		int directional_light_count;
		PerDirectionalUniform directional_lights[2];
	};

	struct Lights
	{
		glm::vec3 Intensity;
		float range;
		glm::vec3 Position;
		float cone_angle;
		glm::vec3 Direction;
		float padding;
		glm::mat4 light_model_xform;
	};


	Mesh light_quad_mesh_; // vertex array of vec2 position
	Mesh light_sphere_mesh_; // element array into vec3 position
	Mesh light_cone_mesh;

	GLuint gbuffer_position_tex_{ 0 };
	GLuint gbuffer_normal_tex_{ 0 };
	GLuint gbuffer_depth_tex_{ 0 };

	GLuint shadow_depth_tex{ 0 };

	GLuint lbuffer_fbo_{ 0 };
	GLuint gbuffer_fbo_{ 0 };
	GLuint shadow_fbo_{ 0 };
	GLuint lbuffer_colour_rbo_{ 0 };
	GLuint gbuffer_colour_rbo_{ 0 };

	std::unordered_map<sponza::MeshId, MeshGL> meshes_;



};
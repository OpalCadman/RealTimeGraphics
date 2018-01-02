#include "MyView.hpp"

#include <sponza/sponza.hpp>
#include <tygra/FileHelper.hpp>
#include <tsl/shapes.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <cassert>


struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
};

MyView::MyView()
{
}

MyView::~MyView() {
}

void MyView::createquadmesh()
{
	std::vector<glm::vec2> vertices(4);
	vertices[0] = glm::vec2(-1, -1);
	vertices[1] = glm::vec2(1, -1);
	vertices[2] = glm::vec2(1, 1);
	vertices[3] = glm::vec2(-1, 1);

	glGenBuffers(1, &light_quad_mesh_.vertex_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, light_quad_mesh_.vertex_vbo);
	glBufferData(GL_ARRAY_BUFFER,
		vertices.size() * sizeof(glm::vec2),
		vertices.data(),
		GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenVertexArrays(1, &light_quad_mesh_.vao);
	glBindVertexArray(light_quad_mesh_.vao);
	glBindBuffer(GL_ARRAY_BUFFER, light_quad_mesh_.vertex_vbo);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
		sizeof(glm::vec2), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void MyView::createspheremesh()
{
	tsl::IndexedMeshPtr mesh = tsl::createSpherePtr(1.f, 12);
	mesh = tsl::cloneIndexedMeshAsTriangleListPtr(mesh.get());

	light_sphere_mesh_.element_count = mesh->indexCount();

	glGenBuffers(1, &light_sphere_mesh_.vertex_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, light_sphere_mesh_.vertex_vbo);
	glBufferData(GL_ARRAY_BUFFER,
		mesh->vertexCount() * sizeof(glm::vec3),
		mesh->positionArray(),
		GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &light_sphere_mesh_.element_vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, light_sphere_mesh_.element_vbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		mesh->indexCount() * sizeof(unsigned int),
		mesh->indexArray(),
		GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glGenVertexArrays(1, &light_sphere_mesh_.vao);
	glBindVertexArray(light_sphere_mesh_.vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, light_sphere_mesh_.element_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, light_sphere_mesh_.vertex_vbo);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
		sizeof(glm::vec3), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void MyView::createconemesh()
{
	tsl::IndexedMeshPtr mesh = tsl::createConePtr(1, 1, 6);
	mesh = tsl::cloneIndexedMeshAsTriangleListPtr(mesh.get());

	light_cone_mesh.element_count = mesh->indexCount();

	glGenBuffers(1, &light_cone_mesh.vertex_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, light_cone_mesh.vertex_vbo);
	glBufferData(GL_ARRAY_BUFFER,
		mesh->vertexCount() * sizeof(glm::vec3),
		mesh->positionArray(),
		GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &light_cone_mesh.element_vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, light_cone_mesh.element_vbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		mesh->indexCount() * sizeof(unsigned int),
		mesh->indexArray(),
		GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glGenVertexArrays(1, &light_cone_mesh.vao);
	glBindVertexArray(light_cone_mesh.vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, light_cone_mesh.element_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, light_cone_mesh.vertex_vbo);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
		sizeof(glm::vec3), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void MyView::setScene(const sponza::Context * scene)
{
    scene_ = scene;
}

void MyView::createShader(GLuint &shader_, GLenum shaderType, std::string stringFromFile)
{
	GLint compile_status = 0;

	shader_ = glCreateShader(shaderType);
	std::string vs_string = tygra::createStringFromFile(stringFromFile);
	const char *vs_code = vs_string.c_str();
	glShaderSource(shader_, 1, (const GLchar **)&vs_code, NULL);
	glCompileShader(shader_);
	glGetShaderiv(shader_, GL_COMPILE_STATUS, &compile_status);
	if (compile_status != GL_TRUE)
	{
		const int string_length = 1024;
		GLchar log[string_length] = "";
		glGetShaderInfoLog(shader_, string_length, NULL, log);
		std::cerr << log << std::endl;
	}
}

void MyView::createProgram(GLuint &program_name, GLuint &vs, GLuint &Fragemnet_shader)
{
	program_name = glCreateProgram();
	glAttachShader(program_name, vs);
	glBindAttribLocation(program_name, 0, "vertex_position");
	//glDeleteShader(vs);
	glAttachShader(program_name, Fragemnet_shader);
	glBindFragDataLocation(program_name, 0, "fragment_colour");
	//glDeleteShader(Fragemnet_shader);
	glLinkProgram(program_name);

	GLint link_status = 0;
	glGetProgramiv(program_name, GL_LINK_STATUS, &link_status);
	if (link_status != GL_TRUE)
	{
		const int string_length = 1024;
		GLchar log[string_length] = "";
		glGetShaderInfoLog(program_name, string_length, NULL, log);
		std::cerr << log << std::endl;
	}
}

void MyView::drawSponza(GLuint &program_)
{
	GLint viewport_size[4];
	glGetIntegerv(GL_VIEWPORT, viewport_size);
	const float aspect_ratio = viewport_size[2] / (float)(viewport_size[3]);

	glm::mat4 projection_xform = glm::perspective(glm::radians(scene_->getCamera().getVerticalFieldOfViewInDegrees()),
		aspect_ratio,
		scene_->getCamera().getNearPlaneDistance(), scene_->getCamera().getFarPlaneDistance());

	glm::mat4 view_xform = glm::lookAt((const glm::vec3 &)scene_->getCamera().getPosition(),
		(const glm::vec3 &)scene_->getCamera().getPosition() + (const glm::vec3 &)scene_->getCamera().getDirection(),
		(const glm::vec3 &)scene_->getUpDirection());

	glUseProgram(program_);
	glBindVertexArray(vao_);


	PerModelUniforms per_model_uniforms[16];

	view_projection = projection_xform * view_xform;


	for (const auto& mesh : meshes_)
	{
		const auto& instanceIds = scene_->getInstancesByMeshId(mesh.first);

		const MeshGL& meshGl = mesh.second;

		for (int i = 0; i < instanceIds.size(); i++)
		{
			const auto& instances = scene_->getInstanceById(instanceIds[i]);
			glm::mat4 model_xform = glm::mat4((const glm::mat4x3 &)instances.getTransformationMatrix());
			per_model_uniforms[i].model_xform = model_xform;
			per_model_uniforms[i].projection_view_model_xform = view_projection * model_xform;
		}

		glBindBuffer(GL_UNIFORM_BUFFER, per_model_ubo);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(PerModelUniforms) * instanceIds.size(), &per_model_uniforms);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		glDrawElementsInstancedBaseVertex(GL_TRIANGLES, meshGl.element_count, GL_UNSIGNED_INT, (void*)(meshGl.first_element_index * sizeof(unsigned int)), instanceIds.size(), meshGl.first_vertex_index);
	}


}

void MyView::windowViewWillStart(tygra::Window * window)
{
	assert(scene_ != nullptr);


	createquadmesh();
	createspheremesh();
	createconemesh();

	createShader(spot_light_fs, GL_FRAGMENT_SHADER, "resource:///spot_light_fs.glsl");

	createShader(point_light_vs, GL_VERTEX_SHADER, "resource:///reprise_light_vs.glsl");
	createShader(point_light_fs, GL_FRAGMENT_SHADER, "resource:///reprise_light_fs.glsl");

	createShader(gbuffer_vs, GL_VERTEX_SHADER, "resource:///reprise_gbuffer_vs.glsl");
	createShader(gbuffer_fs, GL_FRAGMENT_SHADER, "resource:///reprise_gbuffer_fs.glsl");

	createShader(global_light_vs, GL_VERTEX_SHADER, "resource:///global_light_vs.glsl");
	createShader(global_light_fs, GL_FRAGMENT_SHADER, "resource:///global_light_fs.glsl");
	
	createProgram(point_light_program_, point_light_vs, point_light_fs);

	createProgram(spot_light_program_, point_light_vs, spot_light_fs);

	createProgram(global_light_program, global_light_vs, global_light_fs);

	
	//Creating new program that adds two out variables to the gbuffer
	gbuffer_program_ = glCreateProgram();
	glAttachShader(gbuffer_program_, gbuffer_vs);
	glBindAttribLocation(gbuffer_program_, 0, "vertex_position");
	glBindAttribLocation(gbuffer_program_, 1, "normal");
	glAttachShader(gbuffer_program_, gbuffer_fs);
	glBindFragDataLocation(gbuffer_program_, 0, "Positions");
	glBindFragDataLocation(gbuffer_program_, 1, "Normals");
	glLinkProgram(gbuffer_program_);

	glDeleteShader(gbuffer_vs);
	glDeleteShader(gbuffer_fs);
	glDeleteShader(spot_light_fs);
	glDeleteShader(point_light_vs);
	glDeleteShader(point_light_fs);
	glDeleteShader(global_light_vs);
	glDeleteShader(global_light_fs);


	GLint link_status = 0;
	glGetProgramiv(gbuffer_program_, GL_LINK_STATUS, &link_status);
	if (link_status != GL_TRUE)
	{
		const int string_length = 1024;
		GLchar log[string_length] = "";
		glGetShaderInfoLog(gbuffer_program_, string_length, NULL, log);
		std::cerr << log << std::endl;
	}

	link_status = 0;
	glGetProgramiv(global_light_program, GL_LINK_STATUS, &link_status);
	if (link_status != GL_TRUE)
	{
		const int string_length = 1024;
		GLchar log[string_length] = "";
		glGetShaderInfoLog(global_light_program, string_length, NULL, log);
		std::cerr << log << std::endl;
	}



	sponza::GeometryBuilder builder;
	const auto& scene_meshes = builder.getAllMeshes();

	std::vector<Vertex> vertices;
	std::vector<unsigned int> allElements;

	for (const auto& scene_mesh : scene_meshes)
	{
		// Get data.
		MeshGL& newMesh = meshes_[scene_mesh.getId()];
		const auto& positions = scene_mesh.getPositionArray();
		const auto& normals = scene_mesh.getNormalArray();
		const auto& elements = scene_mesh.getElementArray();

		// Element data.
		newMesh.first_element_index = allElements.size();
		newMesh.element_count = elements.size();
		allElements.insert(allElements.end(), elements.begin(), elements.end());

		// Vertex data.
		newMesh.first_vertex_index = vertices.size();
		for (int i = 0; i < positions.size(); i++)
		{
			Vertex vertex;

			vertex.position = (const glm::vec3 &)positions[i];
			vertex.normal = (const glm::vec3 &)normals[i];

			vertices.push_back(vertex);
		}
	}


	glGenTextures(1, &gbuffer_position_tex_);
	glGenTextures(1, &gbuffer_normal_tex_);
	glGenTextures(1, &gbuffer_depth_tex_);

	glGenFramebuffers(1, &lbuffer_fbo_);
	glGenFramebuffers(1, &gbuffer_fbo_);
	glGenRenderbuffers(1, &lbuffer_colour_rbo_);
	glGenRenderbuffers(1, &gbuffer_colour_rbo_);


	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glPixelStorei(GL_UNPACK_SWAP_BYTES, GL_TRUE);

	// Create vertex buffer object.
	glGenBuffers(1, &vertex_vbo_);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_vbo_);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Create element buffer object.
	glGenBuffers(1, &element_vbo_);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_vbo_);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, allElements.size() * sizeof(unsigned int), allElements.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//Create Uniform Buffer for models
	glGenBuffers(1, &per_model_ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, per_model_ubo);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(PerModelUniforms) * 16, nullptr, GL_STREAM_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	
	glGenBuffers(1, &global_light_ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, global_light_ubo);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(global_lights), nullptr, GL_STREAM_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glGenBuffers(1, &point_light_ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, point_light_ubo);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(Lights), nullptr, GL_STREAM_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	global_lights global_light;

	const auto& directionalLights = scene_->getAllDirectionalLights();

	for (size_t i = 0; i < directionalLights.size(); ++i)
	{
		global_light.directional_lights[i].direction = (const glm::vec3&)directionalLights[i].getDirection();
		global_light.directional_lights[i].intensity = (const glm::vec3&)directionalLights[i].getIntensity();
	}

	global_light.Ambient_Intensity = (const glm::vec3&)scene_->getAmbientLightIntensity();
	global_light.directional_light_count = directionalLights.size();

	glBindBuffer(GL_UNIFORM_BUFFER, global_light_ubo);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(global_lights), &global_light);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	// Creating Vertex Array Object.
	glGenVertexArrays(1, &vao_);
	glBindVertexArray(vao_);


	///Don't forget to add perModelUniform to VS
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, per_model_ubo);
	glUniformBlockBinding(gbuffer_program_, glGetUniformBlockIndex(gbuffer_program_, "PerModelUniform"), 0);

	glBindBufferBase(GL_UNIFORM_BUFFER, 1, global_light_ubo);
	glUniformBlockBinding(global_light_program, glGetUniformBlockIndex(global_light_program, "global_lights"), 1);

	glBindBufferBase(GL_UNIFORM_BUFFER, 2, point_light_ubo);
	glUniformBlockBinding(point_light_program_, glGetUniformBlockIndex(point_light_program_, "point_lights"), 2);
	glUniformBlockBinding(spot_light_program_, glGetUniformBlockIndex(spot_light_program_, "point_lights"), 2);

	glBindBuffer(GL_ARRAY_BUFFER, vertex_vbo_);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_vbo_);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), TGL_BUFFER_OFFSET_OF(Vertex, position));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), TGL_BUFFER_OFFSET_OF(Vertex, normal));
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glClearStencil(0x80);
	glClearDepth(1.0);
}


void MyView::windowViewDidReset(tygra::Window * window,
                                int width,
                                int height)
{
	// override actual window size to match gbuffer image data

    glViewport(0, 0, width, height);

	glBindRenderbuffer(GL_RENDERBUFFER, lbuffer_colour_rbo_);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, width, height);

	glBindRenderbuffer(GL_RENDERBUFFER, gbuffer_colour_rbo_);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, width, height);

	glBindTexture(GL_TEXTURE_RECTANGLE, gbuffer_depth_tex_);
	glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, 0);

	glBindTexture(GL_TEXTURE_RECTANGLE, gbuffer_position_tex_);
	glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, 0);

	glBindTexture(GL_TEXTURE_RECTANGLE, gbuffer_normal_tex_);
	glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, 0);


	GLenum framebuffer_status = 0;

	// lbuffer fbo
	glBindFramebuffer(GL_FRAMEBUFFER, lbuffer_fbo_);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, lbuffer_colour_rbo_);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_RECTANGLE, gbuffer_depth_tex_, 0);

	framebuffer_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (framebuffer_status != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "Broke Yo" << std::endl;
		assert(false);
	}

	//gbuffer fbo
	glBindFramebuffer(GL_FRAMEBUFFER, gbuffer_fbo_);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE, gbuffer_position_tex_, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_RECTANGLE, gbuffer_normal_tex_, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_RECTANGLE, gbuffer_depth_tex_, 0);
	
	framebuffer_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (framebuffer_status != GL_FRAMEBUFFER_COMPLETE) {
		tglDebugMessage(GL_DEBUG_SEVERITY_HIGH_ARB, "framebuffer not complete");
		assert(false);
	}

	GLenum attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);


}

void MyView::windowViewDidStop(tygra::Window * window)
{
	//glDeleteProgram(light_program_);

	for (const auto& instance : scene_->getAllInstances())
	{
		glDeleteBuffers(1, &vertex_vbo_);
		glDeleteBuffers(1, &element_vbo_);
		glDeleteVertexArrays(1, &vao_);
	}

	glDeleteTextures(1, &gbuffer_position_tex_);
	glDeleteTextures(1, &gbuffer_normal_tex_);
	glDeleteTextures(1, &gbuffer_depth_tex_);

	glDeleteFramebuffers(1, &lbuffer_fbo_);
	glDeleteFramebuffers(1, &gbuffer_fbo_);
	glDeleteRenderbuffers(1, &lbuffer_colour_rbo_);

}

void MyView::windowViewRender(tygra::Window * window)
{
	assert(scene_ != nullptr);

	GLint viewport_size[4];
	glGetIntegerv(GL_VIEWPORT, viewport_size);
	const float aspect_ratio = viewport_size[2] / (float)(viewport_size[3]);
	
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_ALWAYS, 0, ~0);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glCullFace(GL_BACK);

	glDepthMask(GL_TRUE);

	glEnable(GL_DEPTH_TEST);

	glDisable(GL_BLEND);

	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_RECTANGLE, 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_RECTANGLE, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, gbuffer_fbo_);

	glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	drawSponza(gbuffer_program_);

	glBindFramebuffer(GL_FRAMEBUFFER, lbuffer_fbo_);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_RECTANGLE, gbuffer_position_tex_);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_RECTANGLE, gbuffer_normal_tex_);

	
	glClearColor(0.f, 0.f, 0.25f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT);

	glDisable(GL_BLEND);

	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_EQUAL, 0, ~0);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

	glDisable(GL_DEPTH_TEST);

	glUseProgram(global_light_program);

	GLuint world_position = glGetUniformLocation(global_light_program, "sampler_world_position");
	glUniform1i(world_position, 0);

	GLuint normal_position = glGetUniformLocation(global_light_program, "sampler_world_normal");
	glUniform1i(normal_position, 1);
	
	//DRAW Q U A D
	glBindVertexArray(light_quad_mesh_.vao);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glUseProgram(point_light_program_);

	world_position = glGetUniformLocation(point_light_program_, "sampler_world_position");
	glUniform1i(world_position, 0);

	normal_position = glGetUniformLocation(point_light_program_, "sampler_world_normal");
	glUniform1i(normal_position, 1);

	Lights pointlight;

	glBindVertexArray(light_sphere_mesh_.vao);

	const auto &pointlights = scene_->getAllPointLights();

	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	glBlendEquation(GL_FUNC_ADD);

	for (int i = 0; i < pointlights.size(); ++i)
	{
		pointlight.Intensity = (const glm::vec3&)pointlights[i].getIntensity();
		pointlight.range = pointlights[i].getRange();
		pointlight.Position = (const glm::vec3&)pointlights[i].getPosition();
		
		auto t = glm::translate(glm::mat4(), pointlight.Position);
		auto s = glm::scale(glm::mat4(), glm::vec3(pointlight.range));
		pointlight.light_model_xform = view_projection * t * s;

		glBindBuffer(GL_UNIFORM_BUFFER, point_light_ubo);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Lights), &pointlight);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		glDrawElements(GL_TRIANGLES, light_sphere_mesh_.element_count, GL_UNSIGNED_INT, 0);
	}

	Lights spotlight;

	glUseProgram(spot_light_program_);

	world_position = glGetUniformLocation(spot_light_program_, "sampler_world_position");
	glUniform1i(world_position, 0);

	normal_position = glGetUniformLocation(spot_light_program_, "sampler_world_normal");
	glUniform1i(normal_position, 1);

	glBindVertexArray(light_cone_mesh.vao);

	const auto &spotlights = scene_->getAllSpotLights();

	for (int i = 0; i < spotlights.size(); ++i)
	{
		/* ******************************************************************************************************* 
			Make sure you refactor before starting shadows, AA or SSR!!!!!
			******************************************************************************************************
		
		*/
		spotlight.Intensity = (const glm::vec3&)spotlights[i].getIntensity();
		spotlight.range = spotlights[i].getRange();
		spotlight.Position = (const glm::vec3&)spotlights[i].getPosition();
		spotlight.cone_angle = glm::radians(spotlights[i].getConeAngleDegrees() / 2.f);
		spotlight.Direction = (const glm::vec3&)spotlights[i].getDirection();

		auto t = glm::translate(glm::mat4(), glm::vec3(0, 0, -1));
		auto a = spotlight.range * glm::tan(spotlight.cone_angle);
		auto s = glm::scale(glm::mat4(), glm::vec3(a, a, spotlight.range));
		auto r = glm::inverse(glm::lookAt(spotlight.Position, spotlight.Position + spotlight.Direction, glm::vec3(0, 1, 0)));
		spotlight.light_model_xform = view_projection * r * s * t;

		glBindBuffer(GL_UNIFORM_BUFFER, point_light_ubo);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Lights), &spotlight);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		glDrawElements(GL_TRIANGLES, light_cone_mesh.element_count, GL_UNSIGNED_INT, 0);
	}

	glBindFramebuffer(GL_READ_FRAMEBUFFER, lbuffer_fbo_);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	glBlitFramebuffer(0, 0, viewport_size[2], viewport_size[3], 0, 0, viewport_size[2], viewport_size[3], GL_COLOR_BUFFER_BIT, GL_NEAREST);
}
//My framebuffer setup including functions which allow for post-processing, render-to-texture, debug-stage blitting, and shadow mapping

#include "framebuffer.h"


std::deque<Framebuffer> Framebuffer::instances = {};


Framebuffer* Framebuffer::init(unsigned int width, unsigned int height, bool include_depth, bool include_render_buffer, bool include_stencil_buffer) {
	frame_buffer = 0;
	render_buffer = 0;
	color = 0;
	depth = 0;
	stencil = 0;
	this->width = width;
	this->height = height;


	glGenFramebuffers(1, &frame_buffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);

	glGenTextures(1, &color);
	glBindTexture(GL_TEXTURE_2D, color);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//s = width
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);//t = height
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);//when rendered tex is smaller/bigger, how is it presented?
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE); // automatic mipmap
	unsigned char* data = 0;
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color, 0);//last param is mipmap

	if (frame_buffer == NULL) return this;

	if (include_depth) {
		if (include_render_buffer) {
			glGenRenderbuffers(1, &render_buffer);
			glBindRenderbuffer(GL_RENDERBUFFER, render_buffer);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, render_buffer);
		}
		else {
			glGenTextures(1, &depth);
			glBindTexture(GL_TEXTURE_2D, depth);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//s = width
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);//t = height
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);//when rendered tex is smaller/bigger, how is it presented?
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			//glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE); // automatic mipmap
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth, 0);//last param is mipmap
		}
	}
	const GLenum buffers[] = {GL_COLOR_ATTACHMENT0, include_depth ? GL_DEPTH_ATTACHMENT : GL_NONE, include_stencil_buffer ? GL_STENCIL_ATTACHMENT : GL_NONE};
	glDrawBuffers(3, buffers);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "The framebuffer's been framed and fled the country, would not load" << std::endl;
		//TODO: delete
		//frame_buffer = NULL;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	return this;
}
Framebuffer* Framebuffer::init_for_depth(unsigned int width, unsigned int height) {
	frame_buffer = 0;
	render_buffer = 0;
	color = 0;
	depth = 0;
	stencil = 0;
	this->width = width;
	this->height = height;

	glGenFramebuffers(1, &frame_buffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
	glGenTextures(1, &depth);
	glBindTexture(GL_TEXTURE_2D, depth);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);//s = width
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);//t = height
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT24, width, height);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth, 0);
	//glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &fmt);
	//glDrawBuffer(GL_NONE);
	glDrawBuffer(GL_DEPTH_ATTACHMENT);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "The framebuffer's been framed and fled the country, would not load" << std::endl;
		//TODO: delete
		//frame_buffer = NULL;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	return this;
}
Framebuffer* Framebuffer::init_for_variance(unsigned int width, unsigned int height) {
	frame_buffer = 0;
	render_buffer = 0;
	color = 0;
	depth = 0;
	stencil = 0;
	this->width = width;
	this->height = height;

	glGenFramebuffers(1, &frame_buffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
	glGenTextures(1, &color);
	glBindTexture(GL_TEXTURE_2D, color);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);//s = width
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);//t = height
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, width, height, 0, GL_RGBA, GL_FLOAT, 0);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RG32F, width, height);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, color, 0);
	const GLenum buffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, buffers);

	//Necessary for depth test
	glGenRenderbuffers(1, &render_buffer);
	glBindRenderbuffer(GL_RENDERBUFFER, render_buffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, render_buffer);



	//glDrawBuffer(GL_NONE);//Not necessary

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "The framebuffer's been framed and fled the country, would not load" << std::endl;
		//TODO: delete
		//frame_buffer = NULL;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	return this;
}
Framebuffer* Framebuffer::init_for_render(unsigned int width, unsigned int height) {
	frame_buffer = 0;
	render_buffer = 0;
	color = 0;
	depth = 0;
	stencil = 0;
	this->width = width;
	this->height = height;

	glGenFramebuffers(1, &frame_buffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
	glGenTextures(1, &color);
	glBindTexture(GL_TEXTURE_2D, color);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);//NEAREST
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);//s = width
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);//t = height
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_FLOAT, 0);
	//glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA, width, height);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, color, 0);
	const GLenum buffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, buffers);

	//Necessary for depth test
	glGenRenderbuffers(1, &render_buffer);
	glBindRenderbuffer(GL_RENDERBUFFER, render_buffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, render_buffer);


	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "The framebuffer's been framed and fled the country, would not load" << std::endl;
		//TODO: delete
		//frame_buffer = NULL;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	return this;
}




Framebuffer::~Framebuffer() {
	//if (frame_buffer != NULL) std::cout << "~!Deleting framebuffer at " << frame_buffer << std::endl;
	if (stencil) glDeleteTextures(1, &stencil);
	if (depth) glDeleteTextures(1, &depth);
	if (color) glDeleteTextures(1, &color);
	if (render_buffer) glDeleteRenderbuffers(1, &render_buffer);
	if (frame_buffer) glDeleteFramebuffers(1, &frame_buffer);
}

void Framebuffer::ChangeResolution(unsigned int width, unsigned int height) {
	if (!valid() || (width == this->width && height == this->height))
		return;
	this->width = width;
	this->height = height;
	glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
	glBindTexture(GL_TEXTURE_2D, color);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	if (render_buffer) {
		glBindRenderbuffer(GL_RENDERBUFFER, render_buffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	}
	else if (depth) {
		glBindTexture(GL_TEXTURE_2D, depth);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	}
	if (stencil) {
		glBindTexture(GL_TEXTURE_2D, stencil);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	}
}

bool Framebuffer::valid() {
	return frame_buffer != NULL;
}

void Framebuffer::Bind(unsigned int index, unsigned int slot) {
	//Index can be used for depth/stencil, but not for now
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, color);
	glGenerateMipmap(GL_TEXTURE_2D);
}
//First call app.Clear(), followed by this, and followed by another app.Clear(), then render the scene;
void Framebuffer::BindForDrawing(GLuint reader) {
	if (frame_buffer == NULL) return;
	//glViewport(0, 0, width, height);//Necessary: tells openGL that the camera takes up this much space
	//glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frame_buffer);
	//Potentially need this:
	//const GLenum buffers[] = { GL_COLOR_ATTACHMENT0, depth != 0 ? GL_DEPTH_ATTACHMENT : GL_NONE, stencil != 0 ? GL_STENCIL_ATTACHMENT : GL_NONE };
	//glDrawBuffers(3, buffers);
}
//First render the scene before calling this;
void Framebuffer::BindForReading(GLuint drawer) {
	if (frame_buffer == NULL) return;
	//glViewport(0, 0, 1200, 800);//Necessary: tell openGL what the updated camera size is
	glBindFramebuffer(GL_READ_FRAMEBUFFER, frame_buffer);
	glReadBuffer(GL_COLOR_ATTACHMENT0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, drawer);
	glBlitFramebuffer(0, 0, width, height, 0, 0, 1200, 800, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, drawer);
}




void Framebuffer::gaussian_blur(Framebuffer* primary, Framebuffer* secondary, float intensity) {
	if (primary == NULL || secondary == NULL || primary->frame_buffer == secondary->frame_buffer) return;
	Shader* shader = Shader::find("filter_gaussian");
	if (shader == NULL) shader = Shader::alloc("./res/shader/filter_gaussian");
	shader->bind();
	shader->pass("position", vec4(0.0f, 0.0f, 1.0f, 1.0f));

	int d = primary->width;//ideally switch to height for step 2
	float b = 1.0f / (float)d * intensity;//texel
	//b = intensity / (float)d;//which to use?

	//Run Gaussian Blur Along X Axis onto Temp Texture
	secondary->bind_full();
	vec3 blur = vec3(b, 0.0f, 0.0f);
	shader->pass("blur", blur);
	shader->pass("image", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, primary->main());
	rect::draw();

	//Run Gaussian Blur Along Y Axis onto Main Texture
	primary->bind_full();
	blur = vec3(0.0f, b, 0.0f);
	shader->pass("blur", blur);
	glBindTexture(GL_TEXTURE_2D, secondary->main());
	rect::draw();
}
void Framebuffer::draw_post(Framebuffer* src, Framebuffer* dst) {
	if (src == NULL || dst == NULL || src->frame_buffer == dst->frame_buffer) return;
	dst->bind_full();//Must come before BindTexture
	Shader* shader = Shader::find("filter_null");
	if (shader == NULL) shader = Shader::alloc("./res/shader/filter_null");
	shader->bind();
	shader->pass("position", vec4(0.0f, 0.0f, 1.0f, 1.0f));
	shader->pass("image", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, src->main());
	rect::draw();
}
void Framebuffer::draw_post(Framebuffer* src, ivec2 screen_loc, ivec2 screen_res, float r, float g, float b, float a, bool clear_color, bool clear_depth, bool blit) {
	if (src == NULL || src->frame_buffer == NULL) return;
	src->unbind_full(screen_loc, screen_res, 0, r, g, b, a, clear_color, clear_depth, blit);
	Shader* shader = Shader::find("filter_null");
	if (shader == NULL) shader = Shader::alloc("./res/shader/filter_null");
	shader->bind();
	shader->pass("position", vec4(0.0f, 0.0f, 1.0f, 1.0f));
	shader->pass("image", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, src->main());
	rect::draw();
}
void Framebuffer::fxaa(Framebuffer* src, ivec2 screen_loc, ivec2 screen_res, float r, float g, float b, float a, bool clear_color, bool clear_depth, bool blit) {
	if (src == NULL || src->frame_buffer == NULL) return;
	src->unbind_full(screen_loc, screen_res, 0, r, g, b, a, clear_color, clear_depth, blit);
	Shader* shader = Shader::find("filter_fxaa");
	if (shader == NULL) shader = Shader::alloc("./res/shader/filter_fxaa");
	shader->bind();
	shader->pass("position", vec4(0.0f, 0.0f, 1.0f, 1.0f));
	shader->pass("image", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, src->main());
	rect::draw();
}

//TODO: For Shadow Maps: ClearColor to vec4(1.0f, 1.0f, 0.0f, 0.0f);

Framebuffer* Framebuffer::bind_full(bool clear_color, bool clear_depth) {
	if (frame_buffer == NULL) return this;
	glViewport(0, 0, width, height);
	//glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);//Do this OR both:
	glBindFramebuffer(GL_READ_FRAMEBUFFER, frame_buffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frame_buffer);
	if (clear_color && clear_depth) glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	else if (clear_color) glClear(GL_COLOR_BUFFER_BIT);
	else if (clear_depth) glClear(GL_DEPTH_BUFFER_BIT);
	return this;
}
Framebuffer* Framebuffer::bind_full(float r, float g, float b, float a, bool clear_color, bool clear_depth) {
	if (frame_buffer == NULL) return this;
	glBindTexture(GL_TEXTURE_2D, 0);
	glViewport(0, 0, width, height);
	//glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);//Do this OR both:
	glBindFramebuffer(GL_READ_FRAMEBUFFER, frame_buffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frame_buffer);
	glClearColor(r, g, b, a);
	if (clear_color && clear_depth) glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	else if (clear_color) glClear(GL_COLOR_BUFFER_BIT);
	else if (clear_depth) glClear(GL_DEPTH_BUFFER_BIT);
	return this;
}
Framebuffer* Framebuffer::unbind_full(ivec2 loc, ivec2 res, GLuint dst, bool clear_color, bool clear_depth, bool blit) {
	if (frame_buffer == NULL) return this;
	glViewport(loc.x, loc.y, res.x, res.y);
	//glBindFramebuffer(GL_FRAMEBUFFER, dst);//Does read AND draw; read is necessary for blit and glReadPixels
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dst);//Singular approach if you want blit
	if (clear_color && clear_depth) glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	else if (clear_color) glClear(GL_COLOR_BUFFER_BIT);
	else if (clear_depth) glClear(GL_DEPTH_BUFFER_BIT);
	if (blit) {
		glReadBuffer(GL_COLOR_ATTACHMENT0);//Read the ReadBuffer, assuming you've bound it
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dst);//Set DrawBuffer
		glBlitFramebuffer(0, 0, width, height, loc.x, loc.y, loc.x + res.x, loc.y + res.y, GL_COLOR_BUFFER_BIT, GL_NEAREST);//Blit
	}
	//glBindFramebuffer(GL_FRAMEBUFFER, dst);//Viewport's buffer MUST BE set AFTER
	return this;
}
Framebuffer* Framebuffer::unbind_full(ivec2 loc, ivec2 res, GLuint dst, float r, float g, float b, float a, bool clear_color, bool clear_depth, bool blit) {
	if (frame_buffer == NULL) return this;
	glBindTexture(GL_TEXTURE_2D, 0);
	glViewport(loc.x, loc.y, res.x, res.y);
	//glBindFramebuffer(GL_FRAMEBUFFER, dst);//Viewport's buffer MUST BE set AFTER
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dst);//Singular approach if you want blit
	glClearColor(r, g, b, a);
	if (clear_color && clear_depth) glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	else if (clear_color) glClear(GL_COLOR_BUFFER_BIT);
	else if (clear_depth) glClear(GL_DEPTH_BUFFER_BIT);
	if (blit) {
		if (color != NULL) glReadBuffer(GL_COLOR_ATTACHMENT0);//Read the ReadBuffer, assuming you've bound it
		else glReadBuffer(GL_DEPTH_ATTACHMENT);
		glBlitFramebuffer(0, 0, width, height, loc.x, loc.y, loc.x + res.x, loc.y + res.y, color != NULL ? GL_COLOR_BUFFER_BIT : GL_DEPTH_BUFFER_BIT, GL_NEAREST);//Blit
	}
	return this;
}




Framebuffer* Framebuffer::find(std::string name) {
	return NULL;
}
Framebuffer* Framebuffer::alloc() {
	int index = instances.size();
	auto generator = []() {return Framebuffer(); };
	std::generate_n(std::back_inserter(instances), 1, generator);
	Framebuffer* output = &instances.at(index);
	std::cout << "> Alloc Framebuffer at index " << index << std::endl;
	return output;
}
Framebuffer* Framebuffer::alloc(unsigned int width, unsigned int height, bool include_depth, bool include_render_buffer, bool include_stencil_buffer) {
	Framebuffer* output = alloc();
	output->init(width, height, include_depth, include_render_buffer, include_stencil_buffer);
	return output;
}
Framebuffer* Framebuffer::alloc_depth(unsigned int width, unsigned int height) {
	Framebuffer* output = alloc();
	output->init_for_depth(width, height);
	return output;
}
Framebuffer* Framebuffer::alloc_variance(unsigned int width, unsigned int height) {
	Framebuffer* output = alloc();
	output->init_for_variance(width, height);
	return output;
}
Framebuffer* Framebuffer::alloc_render(unsigned int width, unsigned int height) {
	Framebuffer* output = alloc();
	output->init_for_render(width, height);
	return output;
}



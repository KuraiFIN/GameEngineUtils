//My framebuffer setup including functions which allow for post-processing, render-to-texture, debug-stage blitting, and shadow mapping

#pragma once

#include<iostream>
#include<deque>
#include<algorithm>
#include<GL/glew.h>


#include "vector.h"

#include "shader.h"

class Framebuffer
{
public:

	GLuint frame_buffer = 0;
	GLuint render_buffer = 0;

	unsigned int width;
	unsigned int height;

	bool valid();
	Framebuffer* init(unsigned int width, unsigned int height, bool include_depth = true, bool include_render_buffer = true, bool include_stencil_buffer = false);
	Framebuffer* init_for_depth(unsigned int width, unsigned int height);
	Framebuffer* init_for_variance(unsigned int width, unsigned int height);
	Framebuffer* init_for_render(unsigned int width, unsigned int height);

	void ChangeResolution(unsigned int width, unsigned int height);

	void Bind(unsigned int index, unsigned int slot);

	void BindForDrawing(GLuint reader = 0);
	void BindForReading(GLuint drawer = 0);


	static void gaussian_blur(Framebuffer* primary, Framebuffer* secondary, float intensity);
	static void draw_post(Framebuffer* src, Framebuffer* dst);
	static void draw_post(Framebuffer* src, ivec2 screen_loc, ivec2 screen_res, float r, float g, float b, float a = 1.0f, bool clear_color = true, bool clear_depth = true, bool blit = false);
	static void fxaa(Framebuffer* src, ivec2 screen_loc, ivec2 screen_res, float r, float g, float b, float a = 1.0f, bool clear_color = true, bool clear_depth = true, bool blit = false);

	Framebuffer* bind_full(bool clear_color = true, bool clear_depth = true);
	Framebuffer* bind_full(float r, float g, float b, float a, bool clear_color = true, bool clear_depth = true);
	Framebuffer* unbind_full(ivec2 loc, ivec2 res, GLuint dst, bool clear_color = true, bool clear_depth = true, bool blit = false);
	Framebuffer* unbind_full(ivec2 loc, ivec2 res, GLuint dst, float r, float g, float b, float a, bool clear_color = true, bool clear_depth = true, bool blit = false);

	~Framebuffer();



	static std::deque<Framebuffer> instances;
	static Framebuffer* find(std::string name);
	static Framebuffer* alloc();
	static Framebuffer* alloc(unsigned int width, unsigned int height, bool include_depth = true, bool include_render_buffer = true, bool include_stencil_buffer = false);
	static Framebuffer* alloc_depth(unsigned int width, unsigned int height);
	static Framebuffer* alloc_variance(unsigned int width, unsigned int height);
	static Framebuffer* alloc_render(unsigned int width, unsigned int height);


	//int a = 5;      // binary: 101
	//int b = a << 3; // binary: 101000, or 40 in decimal
	//int c = b >> 3; // binary: 101, or back to 5 like we started with

	GLuint color = 0;
	GLuint depth = 0;
	GLuint stencil = 0;
	inline GLuint main() {
		return color != NULL ? color : depth;
	}
private:



};


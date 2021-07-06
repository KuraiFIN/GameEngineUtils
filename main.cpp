#pragma warning( push, 0 )

#include <iostream>
#include<string>
#include<thread>

#include "application.h"

#include "camera.h"
#include "light.h"
#include "shader.h"

#include "tree.h"

#include "renderer.h"
#include "primitives.h"

#include "division.h"

#include "time.h"

#include "input.h"

#include "util.h"



//#define width 1200
//#define height 800

int main(int argc, char* argv[])
{
	const int width = 1200;
	const int height = 800;
	Application app("kuraisys", width, height);

	app.window->split(0.2f);
	app.window->b->split(0.75f);
	app.window->b->a->split(0.3f, false);

	app.window->b->a->b->create_camera(70.0f);
	std::cout << app.window->str(width, height, 0) << std::endl;


	division* divp = division::alloc(vec2(1.0f, 1.0f), vec2(0.0f), vec2(0.0f), vec2(0.0f), vec2(0.0f));
	division* div1 = division::alloc(vec2(1.0f, 0.33333333f), vec2(0.0f), vec2(0.0f), vec2(0.5f, 0.0f), vec2(0.5f, 0.0f));
	div1->parent = divp;
	division* div2 = division::alloc(vec2(1.0f, 0.33333333f), vec2(0.0f, 0.33333333f), vec2(0.0f), vec2(0.5f, 0.0f), vec2(0.5f, 0.0f));
	div2->parent = divp;
	div2->colour_main = vec4(0.1f, 0.2f, 0.4f, 1.0f);
	division* div3 = division::alloc(vec2(1.0f, 0.33333333f), vec2(0.0f, 0.66666666f), vec2(0.0f), vec2(0.5f, 0.0f), vec2(0.5f, 0.0f));
	div3->parent = divp;
	div3->colour_main = vec4(0.1f, 0.4f, 0.2f, 1.0f);

	divp->divs.push_back(div1);
	divp->divs.push_back(div2);
	divp->divs.push_back(div3);

	std::cout << divp->str(0, 0, 1200, 800, 0, 0, 0.5f, 0) << std::endl;

  
	tree t = tree("C:\\Users\\Kurai\\incrementum_v0-20-10\\Assets\\skeleton.objs WORKING X-ZY.objs", "o");
	std::cout << "Tree Load:\n" << t.str() << "\nRoot[" << t.root.indices.size() << "]" << std::endl;
	std::cout << "Tree Object: " << Object::alloc(t)->name << std::endl;



	Object::alloc("A");
	Object::alloc("B");
	Object::alloc("C");


	ivec2 wloc = Window::test->location(width, height);
	ivec2 wres = Window::test->resolution(width, height);

	Shader* compute = Shader::alloc_compute("./res/shader/compute");
	Framebuffer* render = Framebuffer::alloc_render(wres.x, wres.y);
	Framebuffer* shadow = Framebuffer::alloc_variance(1024, 1024);
	Framebuffer* shadow_temp = Framebuffer::alloc_variance(1024, 1024);
	double o = 2.0;
	glm::mat4 shadow_persp = glm::orthoRH(-o, o, -o, o, -o, o);//Larger XY = use more pixels, Smaller Z = exaggerate difference
	Transform shadow_trans = Transform();
	shadow_trans.location_local = vec3(0.5f, 0.7f, 0.0f);
	shadow_trans.rotation_local = quat(vec3(0.0f, 1.0f, 0.0f), 90.0f);
	//material->set_int("shadow_map", 2);



	Renderer mesh = Renderer("C:\\Users\\Kurai\\incrementum_v0-20-10\\Data\\output\\male_shape02sup.058.obj"}, 0);
	Material* material = Material::alloc()->with(Shader::alloc("./res/shader/_pbr"))->with(Texture::alloc_thread("C:\\Users\\Kurai\\Documents\\_tex\\src\\cloth.png"), "diffuse_map")->with(Texture::alloc_thread("C:\\Users\\Kurai\\Documents\\_tex\\src\\normal_test.jpg"), "normal_map", 1)->with(shadow->main(), "shadow_map", 2);
	Material* shadow_mat = Material::alloc()->with(Shader::alloc("./res/shader/shadow"));

	Mesh* plane = primitives::plane(Mesh::alloc(), vec3(0.5f, 0.0f, -0.9f), vec3(0.0f, -2.0f, 0.0f), vec3(-0.5f, 0.0f, 0.0f) );



	Transform transform = Transform();
	transform.location_local = vec3(0.0f, -0.5f, 0.0f);
	transform.scale = vec3(2.0f);
	Camera* camera = Camera::alloc("Camera", 70.0f, width / 4, height / 4, 0.01f, 1000.0f);
	camera->object.transform.location_local = vec3(0.0f, 0.7f, 0.75f);
	camera->set_persp(app.window->b->a->b->resolution(width, height).aspect());//Currently *this* cam is the proxy for the window's

	DirLight dirlight(vec3(0.8f, 0.75f, 0.7f), 1.0f, vec3(0.2, -0.4, 0.5f));
	dirlight.AttachToShader(*material, "directional");
	PointLight point(vec3(0.1f, 1.2f, -0.2f), 100.0f, vec3(0.0f, 0.0f, 50.0f), 5.0f);
	point.object.transform.location_local = vec3(1.5f, 0.25f, 1.75f);//OpenGL forward is straight at you!!!
	point.AttachToShader(*material, "points", 0);
	SpotLight spot(vec3(0.7f, 0.2f, 0.1f), 0.3f, vec3(0.0f, 0.0f, 1.0f), 10.0f, vec3(0.0f, 0.0f, 1.0f), 0.9f);
	spot.object.transform.location_local = vec3(0.0f, 0.5f, -4.0f);
	spot.AttachToShader(*material, "spots", 0);


	Object::print_objects();



	
	float timer = 0.0f;

	while (!app.Closing()) {

		//--Prepare Rendering---------------------------------------
		glUseProgram(0);
		glEnable(GL_DEPTH_TEST);


		//We would run this draw section for every window: currently only Window::test
		shadow->bind_full(0.1f, 0.1f, 0.1f, 1.0f);
		shadow_mat->bind()->set_camera(shadow_persp, shadow_trans.matrix_self(), true)->set_model(transform.matrix_self());
		mesh.draw();
		shadow_mat->set_model();
		plane->draw();

		Framebuffer::gaussian_blur(shadow, shadow_temp, 1.0f);
		
		//shadow->unbind_full(Window::test->location(width, height), Window::test->resolution(width, height), 0, 0.1f, 0.1f, 0.1f, 1.0f, true, true, false);//Non Post Processing
		render->bind_full(0.1f, 0.1f, 0.1f, 1.0f);//For Post Processing and FXAA
		material->bind()->set_camera("light", shadow_persp, shadow_trans.matrix_self(), true)->set_camera(camera->perspective, camera->view_matrix())->set_model(transform.matrix_self());
		mesh.draw();
		material->set_model();
		plane->draw();

		compute->with_compute_params(render->main(), render->width, render->height, 8, 8);
		Framebuffer::fxaa(render, wloc, wres, 0.1f, 0.1f, 0.1f);

		

		//--Visualize Window Cameras--------------------------------
		//app.BindReadBuffer(*app.window);

		//__________________________________________________________
		//--Overlay UI----------------------------------------------
		glViewport(0, 0, width, height);
		glDisable(GL_DEPTH_TEST);
		divp->refresh(app.window->b->b);
		rect::draw(vec2(0.25f), vec2(0.2f, 0.1f), vec2(Window::app_width, Window::app_height));
		rect::draw(vec2(0.5f, 0.25f), vec2(0.2f, 0.1f), vec2(Window::app_width, Window::app_height));


		//--Update Test Transform-----------------------------------
		if (Input::key_up(KEY::LCTRL) || Input::key_up(KEY::RCTRL)) std::cout << "CTRL was hit; wow!" <<std::endl;
		//std::cout << Time::get() << std::endl;
		if (Input::mouse_press(mousecode::MMB)) {
			if (Input::mouse_change.x != 0) camera->object.transform.rotation_local = quat::concat(camera->object.transform.rotation_local, quat(vec3(0.0f, 1.0f, 0.0f), Input::mouse_change.x * -0.5f));
			if (Input::mouse_change.y != 0) camera->object.transform.rotation_local = quat::concat(camera->object.transform.rotation_local, quat(camera->object.transform.rotation_local.transform(vec3(1.0f, 0.0f, 0.0f)), Input::mouse_change.y * 0.5f));
		}
		if (Input::key_press(KEY::W)) camera->object.transform.location_local = camera->object.transform.location_local + (camera->object.transform.rotation_local.transform(vec3(0.0f, 0.0f, -1.0f)) * 0.005f);
		if (Input::key_press(KEY::S)) camera->object.transform.location_local = camera->object.transform.location_local + (camera->object.transform.rotation_local.transform(vec3(0.0f, 0.0f, 1.0f)) * 0.005f);
		if (Input::key_press(KEY::A)) camera->object.transform.location_local = camera->object.transform.location_local + (camera->object.transform.rotation_local.transform(vec3(-1.0f, 0.0f, 0.0f)) * 0.005f);
		if (Input::key_press(KEY::D)) camera->object.transform.location_local = camera->object.transform.location_local + (camera->object.transform.rotation_local.transform(vec3(1.0f, 0.0f, 0.0f)) * 0.005f);
		
		transform.rotation_local = quat(vec3(0.f, 1.0f, 0.0f), timer);
		timer += 0.02f;
		app.Refresh();
	}

	return 0;
}


#pragma warning( pop )

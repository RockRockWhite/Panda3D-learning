#include "pandaFramework.h"
#include "windowFramework.h"
#include "skyBall.h"
#include "asyncTask.h"
#include "asyncTaskManager.h"
#include "genericAsyncTask.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "AmbientLight.h"
#include "LightRampAttrib.h"
#include "CommonFilters.h"
#include "texture.h"
#include "texturePool.h"
#include "textureStage.h"

NodePath scene;
PandaFramework framework;
WindowFramework* window;
CommonFilters* filters;
AsyncTask::DoneStatus example(GenericAsyncTask* task, void* data)
{
	if (task->get_elapsed_time() <= 50 )
	{
		scene.set_hpr(scene.get_hpr().get_x() + 0.1, scene.get_hpr().get_y() , scene.get_hpr().get_z() );
		cout << "glapsed_time=" << task->get_elapsed_time() <<"  x="<< scene.get_hpr().get_x() << "  y=" << scene.get_hpr().get_y() << "  z=" << scene.get_hpr().get_z()<<"\t"<<endl;
		return AsyncTask::DS_cont;
	}
	window->get_render().set_color_off();

	cout << "Done." << endl;
	return AsyncTask::DS_done;
}
int main(int argc,char* argv[])
{
	framework.open_framework(argc, argv);
	framework.set_window_title("Filters");
	window = framework.open_window();

	//载入天空球
	skyBall* sky = new skyBall(window, "textures/skyball/Panorama.jpg");
	TexturePool* texturePool = TexturePool::get_global_ptr();

	//载入模型
	scene = window->load_model(window->get_render(), "models/white/white.egg");
	scene.set_scale(20);

	NodePath wall = window->load_model(scene, "models/wall/wall.egg");
	wall.set_scale(2);
	wall.set_pos(0, -13, 0);
	
	NodePath stone = window->load_model(scene, "models/stone/stone.egg");
	stone.set_scale(0.1);

	NodePath ball=window->load_model(window->get_render(), "models/ball");
	ball.set_scale(20);
	ball.set_pos(0, 0, 150);

	//载入高光纹理
	Texture *stone_gloss_texture = texturePool->load_texture("models/stone/stone_gloss.png");
	Texture *wall_gloss_texture = texturePool->load_texture("models/wall/5_gloss.jpg");
	Texture *rock_gloss_texture = texturePool->load_texture("textures/rock_gloss.jpg");
	TextureStage* textureStage = new TextureStage("Ts");
	textureStage->set_mode(TextureStage::M_modulate_gloss);

	//设置高光纹理
	stone.set_texture(textureStage, stone_gloss_texture);
	scene.set_texture(textureStage, rock_gloss_texture);

	scene.find("**/white/pCube1").set_texture(textureStage, rock_gloss_texture);


	//创建灯光
	PT(PointLight) light = new PointLight("PointLight");
	light->set_attenuation(LVecBase3f(0.7, 0.05, 0));
	light->set_shadow_caster(1000, 512, 512);
	NodePath lightNodePath = window->get_render().attach_new_node(light);
	lightNodePath.set_pos(ball.get_pos().get_x(), ball.get_pos().get_y(), ball.get_pos().get_z() + 15);

	PT(AmbientLight) Alight;
	Alight = new AmbientLight("alight");
	Alight->set_color(LColor(0.2, 0.2, 0.2, 1));
	NodePath AlightNodePath = window->get_render().attach_new_node(Alight);

	PT(DirectionalLight) DLight;
	DLight = new DirectionalLight("DLight");
	NodePath DLightNodePath = window->get_render().attach_new_node(DLight);

	//设置滤镜
	filters = new CommonFilters(window->get_graphics_output(), NodePath(window->get_camera(0)));
	SetBloomParameters b;
	filters->set_bloom(b);
	filters->set_ambient_occlusion(SetAmbientOcclusionParameters());
	filters->set_blur_sharpen(0.7);
	SetVolumetricLightingParameters s = SetVolumetricLightingParameters(ball);
	s.density = 1.0;
	s.decay= 0.98;
	s.exposure = 0.02;
	filters->set_volumetric_lighting(s);

	//模型设置灯光
	window->get_render().set_light(AlightNodePath);
	window->get_render().set_light(lightNodePath);
	window->get_render().set_light(DLightNodePath);
	sky->get_node().set_light_off(AlightNodePath);
	sky->get_node().set_light_off(lightNodePath);
	sky->get_node().set_light_off(DLightNodePath);
	window->get_render().set_shader_auto();

	//创建全局任务管理器
	AsyncTaskManager* taskManager = AsyncTaskManager::get_global_ptr();
	GenericAsyncTask *exampleTask = new GenericAsyncTask("exampleTask", &example, (void*)NULL);

	//添加到任务管理器
	taskManager->add(exampleTask);

	window->setup_trackball();

	//主循环
	framework.main_loop();
	framework.close_framework();

	return 0;
}

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
#include "bulletWorld.h"
#include "bulletPlaneShape.h"
#include "bulletBoxShape.h"
#include "BulletTriangleMesh.h"
#include "BulletTriangleMeshShape.h"
#include "CreatBulletTriangleMesh.h"
#include "load_prc_file.h"

NodePath scene;
PandaFramework framework;
WindowFramework* window;
CommonFilters* filters;
BulletWorld* world;

AsyncTask::DoneStatus example(GenericAsyncTask* task, void* data)
{
	if (task->get_elapsed_time() <= 50)
	{
		scene.set_hpr(scene.get_hpr().get_x() + 0.1, scene.get_hpr().get_y(), scene.get_hpr().get_z());
		cout << "glapsed_time=" << task->get_elapsed_time() << "  x=" << scene.get_hpr().get_x() << "  y=" << scene.get_hpr().get_y() << "  z=" << scene.get_hpr().get_z() << "\t" << endl;
		return AsyncTask::DS_cont;
	}
	window->get_render().set_color_off();

	cout << "Done." << endl;
	return AsyncTask::DS_done;
}

AsyncTask::DoneStatus update(GenericAsyncTask *task, void* data)
{
	ClockObject *clock = ClockObject::get_global_clock();
	world->do_physics(clock->get_dt(), 10, 1.0 / 180.0);
	return AsyncTask::DS_cont;
}

int main(int argc, char* argv[])
{
	framework.open_framework(argc, argv);
	framework.set_window_title("Bullet HelloWorld");
	//load_prc_file_data("", "fullscreen 1");
	load_prc_file_data("", "win-size 1440 900");
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

	NodePath ball = window->load_model(window->get_render(), "models/ball");
	ball.set_scale(20);
	ball.set_pos(0, 0, 150);

	NodePath phyBox = window->load_model(window->get_render(), "models/box");
	phyBox.set_scale(40);
	phyBox.set_pos(-20, -20, -20);

	//载入高光纹理
	Texture *stone_gloss_texture = texturePool->load_texture("models/stone/stone_gloss.png");
	Texture *wall_gloss_texture = texturePool->load_texture("models/wall/5_gloss.jpg");
	TextureStage* textureStage = new TextureStage("Ts");
	textureStage->set_mode(TextureStage::M_modulate_gloss);

	//设置高光纹理
	stone.set_texture(textureStage, stone_gloss_texture);
	wall.set_texture(textureStage, wall_gloss_texture);

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
	s.decay = 0.98;
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

	//创建物理世界
	world = new BulletWorld();

	//设置重力
	world->set_gravity(0, 0, -9.81f);

	//创建物理性状和刚体
	BulletPlaneShape* planeShape = new BulletPlaneShape(LVector3(0, 0, 1), 1);
	PT(BulletRigidBodyNode) planeRigidNode = new BulletRigidBodyNode("Ground");
	BulletBoxShape* boxShape = new BulletBoxShape(LVector3(20, 20, 20));
	PT(BulletRigidBodyNode) boxRigidNode = new BulletRigidBodyNode("Box");
	PT(BulletTriangleMesh) wallMesh = CreatBulletTriangleMesh::Creat(wall);
	BulletTriangleMeshShape* wallShape = new BulletTriangleMeshShape(wallMesh, false);
	PT(BulletRigidBodyNode) wallRigidNode = new BulletRigidBodyNode("Wall");
	PT(BulletTriangleMesh) sceneMesh = CreatBulletTriangleMesh::Creat(scene.get_child(0));
	BulletTriangleMeshShape* sceneShape = new BulletTriangleMeshShape(sceneMesh,false);
	PT(BulletRigidBodyNode) sceneRigidNode= new BulletRigidBodyNode("scene");
	PT(BulletTriangleMesh)  stoneMesh = CreatBulletTriangleMesh::Creat(scene.get_child(2));
	BulletTriangleMeshShape* stoneShape = new BulletTriangleMeshShape(stoneMesh, false);
	PT(BulletRigidBodyNode) stoneRigidNode = new BulletRigidBodyNode("stone");


	//设置刚体形状
	planeRigidNode->add_shape(planeShape);
	wallRigidNode->add_shape(wallShape);
	boxRigidNode->add_shape(boxShape);
	sceneRigidNode->add_shape(sceneShape);
	stoneRigidNode->add_shape(stoneShape);

	//设置重力
	boxRigidNode->set_mass(10);

	NodePath groundNodePath = window->get_render().attach_new_node(planeRigidNode);
	NodePath boxNodePath = window->get_render().attach_new_node(boxRigidNode);
	NodePath wallNodePath = window->get_render().attach_new_node(wallRigidNode);
	NodePath sceneNodePath = window->get_render().attach_new_node(sceneRigidNode);
	NodePath stoneNodePath = window->get_render().attach_new_node(stoneRigidNode);

	groundNodePath.set_pos(0, 0, 12);
	boxNodePath.set_pos(0, 0, 2);
	
	phyBox.reparent_to(boxNodePath);
	boxNodePath.set_pos(0, 0, 150);

	wallNodePath.set_scale(40, 40, 40);
	wallNodePath.set_pos(0, -260, 0);

	sceneNodePath.set_scale(20,20, 20);
	stoneNodePath.set_scale(2, 2, 2);

	//将刚体加入物理世界
	world->attach(planeRigidNode);
	world->attach(boxRigidNode);
	world->attach(wallRigidNode);
	world->attach(sceneRigidNode);
	world->attach(stoneRigidNode);

	NodePath boxs[1000];
	PT(BulletRigidBodyNode) boxsRigidNode[300];
	NodePath boxsNode[300];

	for (int i = 0; i < 300; i++)
	{
		boxs[i] = window->load_model(window->get_render(), "models/box");
		
		boxs[i].set_scale(40);
		boxs[i].set_pos(-20, -20, -20);
		boxsRigidNode[i] = new BulletRigidBodyNode("BoxRigidNodes"+(char)i);
		boxsRigidNode[i]->add_shape(boxShape);
		boxsRigidNode[i]->set_mass(1);
		boxsNode[i] =window->get_render().attach_new_node(boxsRigidNode[i]);
		boxs[i].reparent_to(boxsNode[i]);
		if(i<=100)
		{
			boxsNode[i].set_pos(0, boxsNode[i-1].get_pos().get_y()-0.2* i, 50 + 100 * i);
		}
		else
		{
			boxsNode[i].set_pos(0, boxsNode[i-1].get_pos().get_y()+0.2*  i, 50 + 100 * i);
		}
		world->attach(boxsRigidNode[i]);
	}

	//Bullet Debug
	BulletDebugNode* debugNode = new BulletDebugNode();
	debugNode->show_bounding_boxes(true);
	debugNode->show_constraints(true);
	debugNode->show_normals(true);
	debugNode->show_wireframe(true);
	world->set_debug_node(debugNode);
	NodePath debugNodePath = window->get_render().attach_new_node(debugNode);
	debugNodePath.set_color(LColor(0, 1, 0, 1));
	debugNodePath.show();

	//创建全局任务管理器
	AsyncTaskManager* taskManager = AsyncTaskManager::get_global_ptr();
	GenericAsyncTask *exampleTask = new GenericAsyncTask("exampleTask", &example, (void*)NULL);
	GenericAsyncTask *updateTask = new GenericAsyncTask("updateTask", &update, (void*)NULL);

	//添加到任务管理器
	//taskManager->add(exampleTask);
	taskManager->add(updateTask);

	window->setup_trackball();

	//主循环
	framework.main_loop();
	framework.close_framework();

	return 0;
}



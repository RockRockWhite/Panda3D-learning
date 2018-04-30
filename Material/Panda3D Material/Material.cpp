#include "pandaFrameWork.h"
#include "windowFramework.h"
#include "auto_bind.h"
#include "animControlCollection.h"
#include "PointLight.h"
#include "ambientLight.h"
#include "Material.h"

int main(int argc, char* argv[])
{
	PandaFramework framework;
	WindowFramework* window;

	//初始化引擎
	framework.open_framework(argc, argv);
	framework.set_window_title("Material");
	window = framework.open_window();

	//载入熊猫模型
	NodePath Actor = window->load_model(window->get_render(), "models/panda-model");
	Actor.set_scale(0.001);
	Actor.set_pos(0, 2.5f, 0);

	//载入动画
	window->load_model(Actor, "models/panda-walk4");
	AnimControlCollection anim;
	auto_bind(Actor.node(),anim);

	//设置循环播放动画
	anim.loop_all(1);

	//创建灯光
	PT(PointLight) PLight;
	PLight = new PointLight("PLight");
	PLight->set_attenuation(LVecBase3(0, 0, 0.5));
	PLight->set_shadow_caster(true);
	NodePath PLightNodePath = window->get_render().attach_new_node(PLight);

	PT(AmbientLight) ALight;
	ALight = new AmbientLight("ALight");
	ALight->set_color(LColor(0.2, 0.2, 0.2, 1));
	NodePath ALightNodePath = window->get_render().attach_new_node(ALight);

	//设置灯光
	window->get_render().set_light(PLightNodePath);
	window->get_render().set_light(ALightNodePath);

	//设置阴影自动
	window->get_render().set_shader_auto();

	//创建材质
	PT(Material) MyMaterial;
	MyMaterial = new Material;

	//设置材质对象
	MyMaterial->set_shininess(5);
	MyMaterial->set_ambient(LColor(0.2, 0.2, 0.2, 1));

	//MyMaterial->set_diffuse(LColor(0,0.33,0, 1));
	MyMaterial->set_specular(LColor(0, 0.33, 0, 1));

	window->get_render().set_material(MyMaterial);

	framework.main_loop();
	framework.close_framework();
}
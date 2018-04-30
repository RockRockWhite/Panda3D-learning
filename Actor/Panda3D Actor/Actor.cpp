#include "pandaFramework.h"
#include "windowFramework.h"
#include "auto_bind.h"
#include "AnimControlCollection.h"
#include "DirectionalLight.h"
#include "pointLight.h"
#include "spotLight.h"
#include "ambientlight.h"
int main(int argc, char* argv[])
{
	PandaFramework framework;
	WindowFramework *window;

	//初始化引擎
	framework.open_framework(argc, argv);
	framework.set_window_title("Lights");
	window = framework.open_window();

	//载入熊猫模型
	NodePath Actor = window->load_model(window->get_render(), "models/panda-model");
	Actor.set_scale(0.001);
	Actor.set_pos(0, 2.5f,0);

	//载入动画
	window->load_model(Actor, "models/panda-walk4");
	AnimControlCollection anim;
	auto_bind(Actor.node(), anim);

	//设置循环播放动画
	anim.loop_all(1);

	//创建定向光
	PT(DirectionalLight) DLight;
	DLight = new DirectionalLight("DLight");
	DLight->set_direction(LVector3(1, 1, 5));//设置定向光方向
	DLight->set_shadow_caster(1,512,512);
	NodePath DLightNodePath = window->get_render().attach_new_node(DLight);

	//创建点光源
	PT(PointLight) PLight;
	PLight = new PointLight("PLight");
	PLight->set_attenuation(LVecBase3(0, 0, 0.5));
	PLight->set_shadow_caster(1, 512, 512);
	NodePath PLightNodePath= window->get_render().attach_new_node(PLight);
	//PLightNodePath.set_pos(500, 500, 500);
	
	//创建聚光灯
	PT(Spotlight)SLight;
	SLight = new Spotlight("SLight");
	SLight->set_color(LColor(0, 0.2, 0, 0.5));
	SLight->set_shadow_caster(1, 512, 512);
	NodePath SLightNodePath = window->get_render().attach_new_node(SLight);

	//创建环境光
	PT(AmbientLight) Alight;
	Alight = new AmbientLight("alight");
	Alight->set_color(LColor(0.2,0.2, 0.2, 1));
	NodePath AlightNodePath = window->get_render().attach_new_node(Alight);

	//灯光作用到模型
	window->get_render().set_light(DLightNodePath);
	window->get_render().set_light(PLightNodePath);
	window->get_render().set_light(SLightNodePath);
	window->get_render().set_light(AlightNodePath);

	//为需要渲染的节点设置阴影
	window->get_render().set_shader_auto();

	//主循环
	framework.main_loop();
	//结束循环后关闭框架
	framework.close_framework(); 

	return 0;
}
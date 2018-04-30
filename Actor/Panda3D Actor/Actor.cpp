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

	//��ʼ������
	framework.open_framework(argc, argv);
	framework.set_window_title("Lights");
	window = framework.open_window();

	//������èģ��
	NodePath Actor = window->load_model(window->get_render(), "models/panda-model");
	Actor.set_scale(0.001);
	Actor.set_pos(0, 2.5f,0);

	//���붯��
	window->load_model(Actor, "models/panda-walk4");
	AnimControlCollection anim;
	auto_bind(Actor.node(), anim);

	//����ѭ�����Ŷ���
	anim.loop_all(1);

	//���������
	PT(DirectionalLight) DLight;
	DLight = new DirectionalLight("DLight");
	DLight->set_direction(LVector3(1, 1, 5));//���ö���ⷽ��
	DLight->set_shadow_caster(1,512,512);
	NodePath DLightNodePath = window->get_render().attach_new_node(DLight);

	//�������Դ
	PT(PointLight) PLight;
	PLight = new PointLight("PLight");
	PLight->set_attenuation(LVecBase3(0, 0, 0.5));
	PLight->set_shadow_caster(1, 512, 512);
	NodePath PLightNodePath= window->get_render().attach_new_node(PLight);
	//PLightNodePath.set_pos(500, 500, 500);
	
	//�����۹��
	PT(Spotlight)SLight;
	SLight = new Spotlight("SLight");
	SLight->set_color(LColor(0, 0.2, 0, 0.5));
	SLight->set_shadow_caster(1, 512, 512);
	NodePath SLightNodePath = window->get_render().attach_new_node(SLight);

	//����������
	PT(AmbientLight) Alight;
	Alight = new AmbientLight("alight");
	Alight->set_color(LColor(0.2,0.2, 0.2, 1));
	NodePath AlightNodePath = window->get_render().attach_new_node(Alight);

	//�ƹ����õ�ģ��
	window->get_render().set_light(DLightNodePath);
	window->get_render().set_light(PLightNodePath);
	window->get_render().set_light(SLightNodePath);
	window->get_render().set_light(AlightNodePath);

	//Ϊ��Ҫ��Ⱦ�Ľڵ�������Ӱ
	window->get_render().set_shader_auto();

	//��ѭ��
	framework.main_loop();
	//����ѭ����رտ��
	framework.close_framework(); 

	return 0;
}
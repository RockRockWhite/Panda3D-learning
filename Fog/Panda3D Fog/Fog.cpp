#include "pandaFrameWork.h"
#include "windowFramework.h"
#include "auto_bind.h"
#include "animControlCollection.h"
#include "PointLight.h"
#include "ambientLight.h"
#include "Fog.h"

int main(int argc, char* argv[])
{
	PandaFramework framework;
	WindowFramework* window;

	//��ʼ������
	framework.open_framework(argc, argv);
	framework.set_window_title("Fog");
	window = framework.open_window();
	window->set_background_type(WindowFramework::BT_white);

	//������èģ��
	NodePath Actor = window->load_model(window->get_render(), "models/panda-model");
	Actor.set_scale(0.001);
	Actor.set_pos(0, 2.5f, 0);

	//���붯��
	window->load_model(Actor, "models/panda-walk4");
	AnimControlCollection anim;
	auto_bind(Actor.node(), anim);

	//����ѭ�����Ŷ���
	anim.loop_all(1);

	//�����ƹ�
	PT(PointLight) PLight;
	PLight = new PointLight("PLight");
	PLight->set_attenuation(LVecBase3(0, 0, 0.5));
	PLight->set_shadow_caster(true);
	NodePath PLightNodePath = window->get_render().attach_new_node(PLight);

	PT(AmbientLight) ALight;
	ALight = new AmbientLight("ALight");
	ALight->set_color(LColor(0.2, 0.2, 0.2, 1));
	NodePath ALightNodePath = window->get_render().attach_new_node(ALight);

	//���õƹ�
	window->get_render().set_light(PLightNodePath);
	window->get_render().set_light(ALightNodePath);

	//������Ӱ�Զ�
	window->get_render().set_shader_auto();

	//������
	PT(Fog) fog;
	fog = new Fog("Fog");
	fog->set_exp_density(0.9);

	Actor.set_fog(fog);

	framework.main_loop();
	framework.close_framework();
}
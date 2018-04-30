#include "pandaFramework.h"
#include "windowFramework.h"
#include "skyBall.h"
#include "asyncTask.h"
#include "asyncTaskManager.h"
#include "genericAsyncTask.h"
#include "DirectionalLight.h"
#include "ambientLight.h"
#include "texture.h"
#include "texturePool.h"
#include "textureStage.h"

NodePath ball;
PandaFramework framework;
WindowFramework* window;

AsyncTask::DoneStatus example(GenericAsyncTask* task, void* data)
{
	if (task->get_elapsed_time() <= 100 )
	{
		ball.set_hpr(ball.get_hpr().get_x() + 0.1, ball.get_hpr().get_y() , ball.get_hpr().get_z() );
		cout << "glapsed_time=" << task->get_elapsed_time() <<"  x="<< ball.get_hpr().get_x() << "  y=" << ball.get_hpr().get_y() << "  z=" << ball.get_hpr().get_z()<<"\t"<<endl;
		return AsyncTask::DS_cont;
	}
	cout << "Done." << endl;
	return AsyncTask::DS_done;
}
int main(int argc,char* argv[])
{
	framework.open_framework(argc, argv);
	framework.set_window_title("Texture");
	window = framework.open_window();

	//���������
	skyBall* sky = new skyBall(window, "textures/skyball/sky_lightblue.jpg");

	//����ģ��
	ball = window->load_model(window->get_render(), "models/ball");
	ball.set_scale(20);

	//���벢���ò���
	TexturePool* texturePool = TexturePool::get_global_ptr();
	Texture* woodText = texturePool->load_texture("textures/wood.jpg");
	ball.set_texture(woodText);

	//���벢���ö�����
	Texture* ironText = texturePool->load_texture("textures/iron.jpg");
	TextureStage* textureStage = new TextureStage("textureStage");
	ball.set_texture(textureStage, ironText);

	//�����ƹ�
	DirectionalLight* DLight;new DirectionalLight("DLight");
	NodePath DLightNodePath = window->get_render().attach_new_node(DLight);

	AmbientLight* ALight = new AmbientLight("ALight");
	ALight->set_color(LColor(0.2f, 0.2f, 0.2f, 1));
	NodePath ALightNodePath = window->get_render().attach_new_node(ALight);
	

	//ģ�����õƹ�
	window->get_render().set_light(DLightNodePath);
	window->get_render().set_light(ALightNodePath);
	sky->get_node().set_light_off(ALightNodePath);
	sky->get_node().set_light_off(DLightNodePath);

	//����Shado�Զ�
	window->get_render().set_shader_auto();

	//����ȫ�����������
	AsyncTaskManager* taskManager = AsyncTaskManager::get_global_ptr();
	GenericAsyncTask *exampleTask = new GenericAsyncTask("exampleTask", &example, (void*)NULL);
	
	//��ӵ����������
	taskManager->add(exampleTask);
	
	window->setup_trackball();

	//��ѭ��
	framework.main_loop();
	framework.close_framework();

	return 0;
}
 
#include "pandaFramework.h"
#include "windowFramework.h"
#include "nodePath.h"
#include "directionalLight.h"
#include "asyncTask.h"
#include "asyncTaskManager.h"
#include "genericAsyncTask.h"
#include "eventHandler.h"

NodePath ball1, ball2;
AsyncTask::DoneStatus scale(GenericAsyncTask* task, void* data)
{
	ball1.set_hpr(ball1.get_hpr().get_x() + 1, 0,0);
	return AsyncTask::DS_again;
}
EventHandler::EventCallbackFunction scales(const Event* task, void* data)
{
	
}

PandaFramework framework;
WindowFramework* window;
int main(int argc, char* argv[])
{
	framework.open_framework(argc, argv);
	framework.set_window_title("Ball");
	window = framework.open_window();

	DirectionalLight* light = new DirectionalLight("");
	NodePath light_n = window->get_render().attach_new_node(light);

	framework.define_key("a","" , scale, (void*)NULL);

	ball1 = window->load_model(window->get_render(),"ball" );
	ball1.get_child(0).set_scale(100);

	ball2 = window->load_model(ball1, "ball");
	ball1.get_child(1).set_scale(10);
	ball1.get_child(1).set_pos(70, 0, 0);

	PT(AsyncTaskManager) taskMgr= AsyncTaskManager::get_global_ptr();
	PT(GenericAsyncTask) task = new GenericAsyncTask("task", &scale,(void*)NULL);
	taskMgr->add(task);


	window->setup_trackball();

	window->get_render().set_light(light_n);
	framework.main_loop();
	framework.close_framework();
	return 0;
}
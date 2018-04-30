#include "pandaFramework.h"
#include "windowFramework.h"
int main(int argc, char* argv[])
{
	PandaFramework framework;
	WindowFramework* window;
	framework.open_framework(argc, argv);
	framework.set_window_title("Node");

	window = framework.open_window();

	NodePath nodePath = window->load_model(window->get_render(), "models/environment");
	//nodePath.reparent_to(window->get_render());
	framework.main_loop();
	framework.close_framework();
	return 0;
}
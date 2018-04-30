#include "pandaFrameWork.h"
#include "windowFrameWork.h"
int main(int argc,char* argv[])

{
	PandaFramework framework;
	WindowFramework* window;
	framework.open_framework(argc, argv);
	framework.set_window_title("Hello World!");
	window = framework.open_window();

	framework.main_loop();
	framework.close_framework();

}
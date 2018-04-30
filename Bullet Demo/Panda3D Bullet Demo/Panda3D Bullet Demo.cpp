#include "pandaFramework.h"
#include "windowFramework.h"
#include "nodePath.h"
#include "clockObject.h"

#include "asyncTask.h"
#include "genericAsyncTask.h"

#include "bulletWorld.h"
#include "bulletDebugNode.h"
#include "bulletPlaneShape.h"
#include "bulletBoxShape.h"

BulletWorld *get_physics_world() {
	// physics_world is supposed to be an global variable,
	// but declaring global variables is not cool
	// for good programmers lol, instead, should use static keyword.
	static BulletWorld *physics_world = new BulletWorld();
	return physics_world;
}

BulletDebugNode *get_physics_debug_node() {
	// Global variable.
	static BulletDebugNode *bullet_dbg_node = new BulletDebugNode("Debug");
	return bullet_dbg_node;
}

void toggle_physics_debug(const Event *e, void *data) {
	static bool show_state = true;
	show_state = !show_state;
	get_physics_debug_node()->show_bounding_boxes(show_state);
	get_physics_debug_node()->show_constraints(show_state);
	get_physics_debug_node()->show_normals(show_state);
	get_physics_debug_node()->show_wireframe(show_state);
}

AsyncTask::DoneStatus update_scene(GenericAsyncTask* task, void* data) {
	// Get dt (from Python example) and apply to do_physics(float, int, int);
	ClockObject *co = ClockObject::get_global_clock();
	get_physics_world()->do_physics(co->get_dt());

	return AsyncTask::DS_cont;
}

int main(int argc, char *argv[]) {
	// All variables.
	PandaFramework framework;
	WindowFramework *window;
	PT(AsyncTaskManager) task_mgr;

	// Init everything :D
	framework.open_framework(argc, argv);
	framework.set_window_title("Bullet Physics");

	window = framework.open_window();
	window->enable_keyboard();
	window->setup_trackball();

	task_mgr = AsyncTaskManager::get_global_ptr();

	// Make physics simulation.
	// Static world stuff.
	get_physics_world()->set_gravity(0, 0, -9.8);

	PT(BulletPlaneShape) floor_shape = new BulletPlaneShape(*new LVecBase3f(0, 0, 1), 1);
	PT(BulletRigidBodyNode) floor_rigid_node = new BulletRigidBodyNode("Ground");

	floor_rigid_node->add_shape(floor_shape);

	NodePath np_ground = window->get_render().attach_new_node(floor_rigid_node);
	np_ground.set_pos(0, 0, -2);
	get_physics_world()->attach(floor_rigid_node);

	// Dynamic world stuff.
	PT(BulletBoxShape) box_shape = new BulletBoxShape(*new LVecBase3f(0.5, 0.5, 0.5));
	PT(BulletRigidBodyNode) box_rigid_node = new BulletRigidBodyNode("Box");

	box_rigid_node->set_mass(1.0); // Gravity affects this rigid node.
	box_rigid_node->add_shape(box_shape);

	NodePath np_box = window->get_render().attach_new_node(box_rigid_node);
	np_box.set_pos(0, 0, 2);
	get_physics_world()->attach(box_rigid_node);

	NodePath np_box_model = window->load_model(framework.get_models(), "models/box");
	np_box_model.set_pos(-0.5, -0.5, -0.5);
	np_box.flatten_light();
	np_box_model.reparent_to(np_box);

	// Debug stuff.
	get_physics_debug_node()->show_bounding_boxes(true);
	get_physics_debug_node()->show_constraints(true);
	get_physics_debug_node()->show_normals(true);
	get_physics_debug_node()->show_wireframe(true);

	NodePath np_dbg_node = window->get_render().attach_new_node(get_physics_debug_node());
	np_dbg_node.show();

	get_physics_world()->set_debug_node(get_physics_debug_node());
	framework.define_key("f1", "Toggle Physics debug", toggle_physics_debug, (void *)NULL);

	// Setup tasks and keys.
	PT(GenericAsyncTask) task;
	task = new GenericAsyncTask("Scene update", &update_scene, (void*)NULL);
	task_mgr->add(task);

	framework.main_loop();
	framework.close_framework();

	return (0);
}
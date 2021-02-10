#ifndef INVOKE_STATE_H
#define INVOKE_STATE_H

#include "core/reference.h"
#include "scene/main/scene_tree.h"

class InvokeState : public Reference {
	GDCLASS(InvokeState, Reference);

	friend class GoostEngine;

	ObjectID instance_id = 0;
	StringName method;
	Ref<SceneTreeTimer> timer; // Needed to fetch `time_left`.

	bool active = false;
	real_t repeat_rate = 0.0;

protected:
	static void _bind_methods();

public:
	Object *get_target() const { return ObjectDB::get_instance(instance_id); }
	StringName get_target_method() const { return method; }

	void cancel() { active = false; }
	bool is_active() { return active; } // No setter, only possible to cancel the state.
	bool is_repeating() { return repeat_rate >= 0.0; }

	real_t get_time_left() { return active ? timer->get_time_left() : 0; }
};

#endif // INVOKE_STATE_H

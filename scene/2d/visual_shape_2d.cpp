#include "visual_shape_2d.h"

#include "core/core_string_names.h"
#include "core/engine.h"

void VisualShape2D::set_shape(const Ref<Shape2D> &p_shape) {
	bool shape_changed = false;
	if (shape != p_shape) {
		shape_changed = true;
	}
	if (shape.is_valid()) {
		shape->disconnect(CoreStringNames::get_singleton()->changed, this, "update");
	}
	shape = p_shape;
	if (shape.is_valid()) {
		shape->connect(CoreStringNames::get_singleton()->changed, this, "update");
	}
	update();

	if (shape_changed) {
		emit_signal("shape_changed");
	}
	update_configuration_warning();
}

Ref<Shape2D> VisualShape2D::get_shape() const {
	return shape;
}

void VisualShape2D::set_use_parent_shape(bool p_use_parent_shape) {
	use_parent_shape = p_use_parent_shape;
	update_parent_shape();
	update_configuration_warning();
	update();
}

bool VisualShape2D::is_using_parent_shape() const {
	return use_parent_shape;
}

bool VisualShape2D::update_parent_shape() {
	bool valid = use_parent_shape;
	bool got_polygon_shape = false;

	if (!is_inside_tree()) {
		valid = false;
	}
	Node *parent = get_parent();
	if (!parent) {
		valid = false;
	}
	if (parent_shape.is_valid()) {
		parent_shape->disconnect(CoreStringNames::get_singleton()->changed, this, "update");
	}
	Ref<Shape2D> previous = parent_shape;

	if (!valid) {
		parent_shape = Ref<Shape2D>();
		return parent_shape != previous;
	}
	parent_shape = parent->get("shape");
	Vector<Vector2> points;
	if (parent_shape.is_null()) {
		points = parent->get("points", &valid);
		if (!valid) {
			points = parent->get("polygon", &valid);
		}
		if (valid) {
			// This might be `CollisionPolygon2D` etc.
			got_polygon_shape = true;
			if (polygon_shape.is_null()) {
				polygon_shape.instance();
			}
			parent_shape = polygon_shape;
		}
	}
	if (parent_shape.is_valid()) {
		if (!parent_shape->is_connected(CoreStringNames::get_singleton()->changed, this, "update")) {
			parent_shape->connect(CoreStringNames::get_singleton()->changed, this, "update");
		}
		// This needs to be set after the shape `changed` signal is connected,
		// so that the polygon shape can be drawn even if `_process` is disabled.
		if (got_polygon_shape) {
			if (points.size() >= 3) {
				polygon_shape->set_points(points);
			} else {
				parent_shape = Ref<Shape2D>();
			}
		}
	}
	return parent_shape != previous;
}

void VisualShape2D::set_color(const Color &p_color) {
	color = p_color;
	update();
}

Color VisualShape2D::get_color() const {
	return color;
}

void VisualShape2D::set_debug_use_default_color(bool p_debug_use_default_color) {
	debug_use_default_color = p_debug_use_default_color;
	update();
}

bool VisualShape2D::is_using_debug_default_color() const {
	return debug_use_default_color;
}

void VisualShape2D::set_debug_sync_visible_collision_shapes(bool p_debug_sync_visible_collision_shapes) {
	debug_sync_visible_collision_shapes = p_debug_sync_visible_collision_shapes;
	update();
}

bool VisualShape2D::is_debug_sync_visible_collision_shapes() const {
	return debug_sync_visible_collision_shapes;
}

void VisualShape2D::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_ENTER_TREE: {
			set_process(true);
		} break;
		case NOTIFICATION_EXIT_TREE: {
			set_process(false);
		} break;
		case NOTIFICATION_PARENTED:
		case NOTIFICATION_PATH_CHANGED: {
			update_parent_shape();
			update();
		} break;
		case NOTIFICATION_DRAW: {
			Ref<Shape2D> draw_shape = shape;
			if (use_parent_shape) {
				// May still need to update the shape if _process() is disabled.
				update_parent_shape();
				draw_shape = parent_shape;
			}
			if (draw_shape.is_null()) {
				break;
			}
			Color draw_color = color;
#ifdef DEBUG_ENABLED
			// Avoid error messages, only in debug to save performance in release.
			Ref<ConvexPolygonShape2D> convex = shape;
			if (convex.is_valid()) {
				if (convex->get_points().size() < 3) {
					break;
				}
			}
			Ref<ConcavePolygonShape2D> concave = shape;
			if (concave.is_valid()) {
				if (concave->get_segments().size() % 2) {
					break;
				}
			}
			// Only relevant in debug builds!
			if (debug_use_default_color) {
				draw_color = get_tree()->get_debug_collisions_color();
			}
			if (debug_sync_visible_collision_shapes && !Engine::get_singleton()->is_editor_hint()) {
				if (!get_tree()->is_debugging_collisions_hint()) {
					break;
				}
			}
#endif
			draw_shape->draw(get_canvas_item(), draw_color);
		} break;
		case NOTIFICATION_PROCESS: {
			if (use_parent_shape) {
				if (update_parent_shape()) {
					update();
				}
			}
		} break;
	}
}

String VisualShape2D::get_configuration_warning() const {
	String warning = Node2D::get_configuration_warning();

	if (shape.is_null() && parent_shape.is_null()) {
		if (!warning.empty()) {
			warning += "\n\n";
		}
		warning += TTR("Shape2D is required for this node to be drawn.");
	}

	return warning;
}

void VisualShape2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_shape", "shape"), &VisualShape2D::set_shape);
	ClassDB::bind_method(D_METHOD("get_shape"), &VisualShape2D::get_shape);

	ClassDB::bind_method(D_METHOD("set_use_parent_shape", "use_parent_shape"), &VisualShape2D::set_use_parent_shape);
	ClassDB::bind_method(D_METHOD("is_using_parent_shape"), &VisualShape2D::is_using_parent_shape);
	ClassDB::bind_method(D_METHOD("update_parent_shape"), &VisualShape2D::update_parent_shape);

	ClassDB::bind_method(D_METHOD("set_color", "color"), &VisualShape2D::set_color);
	ClassDB::bind_method(D_METHOD("get_color"), &VisualShape2D::get_color);

	ClassDB::bind_method(D_METHOD("set_debug_use_default_color", "enable"), &VisualShape2D::set_debug_use_default_color);
	ClassDB::bind_method(D_METHOD("is_using_debug_default_color"), &VisualShape2D::is_using_debug_default_color);

	ClassDB::bind_method(D_METHOD("set_debug_sync_visible_collision_shapes", "debug_sync_visible_collision_shapes"), &VisualShape2D::set_debug_sync_visible_collision_shapes);
	ClassDB::bind_method(D_METHOD("is_debug_sync_visible_collision_shapes"), &VisualShape2D::is_debug_sync_visible_collision_shapes);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "shape", PROPERTY_HINT_RESOURCE_TYPE, "Shape2D"), "set_shape", "get_shape");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "use_parent_shape"), "set_use_parent_shape", "is_using_parent_shape");
	ADD_PROPERTY(PropertyInfo(Variant::COLOR, "color"), "set_color", "get_color");
	ADD_GROUP("Debug", "debug_");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "debug_use_default_color"), "set_debug_use_default_color", "is_using_debug_default_color");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "debug_sync_visible_collision_shapes"), "set_debug_sync_visible_collision_shapes", "is_debug_sync_visible_collision_shapes");

	ADD_SIGNAL(MethodInfo("shape_changed"));
}

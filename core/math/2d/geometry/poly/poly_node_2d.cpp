#include "poly_node_2d.h"
#include "boolean/poly_boolean.h"
#include "decomp/poly_decomp.h"

struct PolyObject2D {
	PolyNode2D *outer;
	Vector<PolyNode2D *> inner;

	Vector<Vector<Point2>> get_polygons_transformed() const {
		Vector<Vector<Point2>> polygons;
		polygons.push_back(outer->get_points_transformed());
		for (int i = 0; i < inner.size(); ++i) {
			polygons.push_back(inner[i]->get_points_transformed());
		}
		return polygons;
	}

	static Vector<PolyObject2D> find_objects(PolyNode2D *p_node) {
		ERR_FAIL_NULL_V(p_node, Vector<PolyObject2D>());

		Vector<PolyObject2D> objects;
		List<PolyNode2D *> to_visit;
		to_visit.push_back(p_node);

		while (!to_visit.empty()) {
			PolyNode2D *n = Object::cast_to<PolyNode2D>(to_visit.back()->get());
			to_visit.pop_back();
			if (!n) {
				continue;
			}
			if (n->is_hole()) {
				for (int i = 0; i < n->get_child_count(); ++i) {
					PolyNode2D *nn = Object::cast_to<PolyNode2D>(n->get_child(i));
					if (!nn) {
						continue;
					}
					to_visit.push_back(nn);
				}
				continue;
			}
			PolyObject2D obj;
			obj.outer = n;

			for (int i = 0; i < n->get_child_count(); ++i) {
				PolyNode2D *nn = Object::cast_to<PolyNode2D>(n->get_child(i));
				if (!nn) {
					continue;
				}
				obj.inner.push_back(nn);
				to_visit.push_back(nn);
			}
			objects.push_back(obj);
		}
		return objects;
	}
};

void draw_polyline_closed(PolyNode2D *p_node, const Vector<Point2> &p_polyline, const Color &p_color, real_t p_width = 1.0) {
	ERR_FAIL_COND(p_polyline.size() < 2);
	p_node->draw_polyline(p_polyline, p_color, p_width);
	p_node->draw_line(p_polyline[p_polyline.size() - 1], p_polyline[0], p_color, p_width);
}

void PolyNode2D::_draw() {
	// Apply final transform, but only on the level of drawing.
	draw_set_transform_matrix(get_global_transform().affine_inverse());

	Vector<PolyObject2D> objects = PolyObject2D::find_objects(this);

	for (int idx = 0; idx < objects.size(); ++idx) {
		const PolyObject2D &obj = objects[idx];
		PolyNode2D *outer = obj.outer;
		const Vector<Point2> &outer_points = outer->get_points_transformed();

		if (outer->open) { // Polyline.
			if (outer_points.size() >= 2) {
				draw_polyline(outer_points, outer->color, outer->width);
			}
		} else if (!outer->filled) { // Non-filled polygon.
			draw_polyline_closed(this, outer_points, outer->color, outer->width);
			for (int i = 0; i < obj.inner.size(); ++i) {
				PolyNode2D *inner = obj.inner[i];
				draw_polyline_closed(this, inner->get_points_transformed(), inner->color, inner->width);
			}
		} else { // Filled polygon.
			const Vector<Vector<Point2>> &polygons = obj.get_polygons_transformed();
			if (polygons.empty()) {
				return;
			}
			const Vector<Vector<Point2>> &triangles = PolyDecomp2D::triangulate_polygons(polygons);
			if (triangles.empty()) {
				break;
			}
			Vector<Point2> vertices;
			for (int i = 0; i < triangles.size(); ++i) {
				const Vector<Point2> &tri = triangles[i];
				for (int j = 0; j < tri.size(); ++j) {
					vertices.push_back(tri[j]);
				}
			}
			const int indices_count = triangles.size() * 3;
			Vector<int> indices;
			for (int i = 0; i < indices_count; ++i) {
				indices.push_back(i);
			}
			Vector<Color> colors;
			colors.push_back(outer->color);
			VS::get_singleton()->canvas_item_add_triangle_array(get_canvas_item(), indices, vertices, colors);
		}
	}
}

void PolyNode2D::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_DRAW: {
			if (!is_inside_tree()) {
				break;
			}
			if (!is_root()) {
				break; // Only the root node is responsible for drawing.
			}
			_draw();
		} break;
		case NOTIFICATION_LOCAL_TRANSFORM_CHANGED: {
			_queue_update();
		} break;
		case NOTIFICATION_PARENTED: {
			_queue_update();
		} break;
	}
}

void PolyNode2D::_queue_update() {
	PolyNode2D *n = this;
	while (n) {
		if (n->is_root()) {
			n->update();
			break;
		}
		n = Object::cast_to<PolyNode2D>(n->get_parent());
	}
}

void PolyNode2D::_validate_property(PropertyInfo &property) const {
	if (property.name == "filled") {
		if (open) {
			property.usage = PROPERTY_USAGE_NOEDITOR;
		}
	}
	if (property.name == "width") {
		if (!open && filled) {
			property.usage = PROPERTY_USAGE_NOEDITOR;
		}
	}
}

void PolyNode2D::_collect_outlines(Vector<Vector<Point2>> *r_closed, Vector<Vector<Point2>> *r_open) {
	ERR_FAIL_COND(!r_closed && !r_open);

	List<PolyNode2D *> to_visit;
	to_visit.push_back(this);

	while (!to_visit.empty()) {
		PolyNode2D *n = Object::cast_to<PolyNode2D>(to_visit.back()->get());
		to_visit.pop_back();
		if (!n) {
			continue;
		}
		const Vector<Point2> &poly = n->get_points_transformed();
		if (r_closed) {
			r_closed->push_back(poly);
		} else if (n->open && r_open) {
			r_open->push_back(poly);
		}
		for (int i = 0; i < n->get_child_count(); ++i) {
			PolyNode2D *nn = Object::cast_to<PolyNode2D>(n->get_child(i));
			if (!nn) {
				continue;
			}
			to_visit.push_back(nn);
		}
	}
}

void PolyNode2D::set_points(const Vector<Point2> &p_points) {
	points = p_points;
	_queue_update();
}

Vector<Point2> PolyNode2D::get_points_transformed() {
	Vector<PolyNode2D *> nodes;
	nodes.push_back(this);
	PolyNode2D *n = Object::cast_to<PolyNode2D>(get_parent());
	while (n) {
		nodes.push_back(n);
		n = Object::cast_to<PolyNode2D>(n->get_parent());
	}
	Vector<Point2> poly = points;
	{
		Point2 *ptr = poly.ptrw();
		for (int i = 0; i < nodes.size(); ++i) {
			const Transform2D &trans = nodes[i]->get_transform();
			for (int j = 0; j < points.size(); ++j) {
				ptr[j] = trans.xform(ptr[j]);
			}
		}
	}
	return poly;
}

void PolyNode2D::set_color(const Color &p_color) {
	color = p_color;
	_queue_update();
}

void PolyNode2D::set_open(bool p_open) {
	open = p_open;
	_queue_update();
	_change_notify();
}

void PolyNode2D::set_filled(bool p_filled) {
	filled = p_filled;
	_queue_update();
	_change_notify();
}

void PolyNode2D::set_width(real_t p_width) {
	width = p_width;
	_queue_update();
}

PolyNode2D *PolyNode2D::new_child(const Vector<Point2> &p_points) {
	PolyNode2D *child = memnew(PolyNode2D);
	child->points = p_points;
	add_child(child);
	return child;
}

bool PolyNode2D::is_hole() const {
	PolyNode2D *n = Object::cast_to<PolyNode2D>(get_parent());
	bool hole = !n && points.empty();
	while (n) {
		hole = !hole;
		n = Object::cast_to<PolyNode2D>(n->get_parent());
	}
	return hole;
}

void PolyNode2D::make_from_outlines(const Array &p_outlines) {
	ERR_FAIL_COND(p_outlines.empty());

	clear();
	Vector<Vector<Point2>> outlines;
	for (int i = 0; i < p_outlines.size(); i++) {
		outlines.push_back(p_outlines[i]);
	}
	PolyNode2D *tree = memnew(PolyNode2D);
	PolyBoolean2D::boolean_polygons_tree(
			outlines, Vector<Vector<Point2>>(), PolyBoolean2D::OP_NONE, tree);

	if (tree->get_child_count() > 0) {
		PolyNode2D *outer = Object::cast_to<PolyNode2D>(tree->get_child(0));
		ERR_FAIL_COND(!outer);
		points = outer->points;
		for (int i = 0; i < outer->get_child_count(); ++i) {
			PolyNode2D *inner = Object::cast_to<PolyNode2D>(outer->get_child(i));
			outer->remove_child(inner);
			add_child(inner);
		}
	}
	memdelete(tree);
	update();
}

Array PolyNode2D::collect_polygons() {
	Vector<Vector<Point2>> polygons;
	_collect_outlines(&polygons, nullptr);

	Array ret;
	for (int i = 0; i < polygons.size(); i++) {
		ret.push_back(polygons[i]);
	}
	return ret;
}

Array PolyNode2D::collect_polylines() {
	Vector<Vector<Point2>> polylines;
	_collect_outlines(nullptr, &polylines);

	Array ret;
	for (int i = 0; i < polylines.size(); i++) {
		ret.push_back(polylines[i]);
	}
	return ret;
}

Array PolyNode2D::find_objects() {
	Array ret;
	Vector<PolyObject2D> objects = PolyObject2D::find_objects(this);
	for (int i = 0; i < objects.size(); ++i) {
		ret.push_back(objects[i].outer);
		// Inner outlines are always children of outer, so no need to retrieve those.
	}
	return ret;
}

Array PolyNode2D::separate_objects(Node *p_new_parent, bool p_keep_transform) {
	ERR_FAIL_COND_V_MSG(!is_inside_tree() && p_keep_transform, Array(),
			"Requested to keep transform, but the node is not inside the scene tree.");
	Array ret;

	Node *new_parent = p_new_parent;
	if (!new_parent) {
		new_parent = get_parent();
	}
	ERR_FAIL_NULL_V_MSG(new_parent, Array(),
			"Invalid parent.");
	ERR_FAIL_COND_V_MSG(new_parent == this, Array(),
			"Cannot create objects inside self.");
	ERR_FAIL_COND_V_MSG(this->is_a_parent_of(new_parent), Array(),
			"Cannot create objects inside nodes which share the same root.");

	Vector<PolyObject2D> objects = PolyObject2D::find_objects(this);
	for (int i = 0; i < objects.size(); ++i) {
		const PolyObject2D &obj = objects[i];
		const PolyNode2D *outer = obj.outer;
		// Boundary.
		PolyNode2D *new_outer = memnew(PolyNode2D);
		new_parent->add_child(new_outer);
		new_outer->points = outer->points;
		new_outer->open = outer->open;
		if (p_keep_transform) {
			new_outer->set_global_transform(outer->get_global_transform());
		}
		// Holes.
		for (int j = 0; j < obj.inner.size(); ++j) {
			const PolyNode2D *inner = obj.inner[j];
			PolyNode2D *new_inner = memnew(PolyNode2D);
			new_inner->points = inner->points;
			new_inner->open = inner->open;
			new_outer->add_child(new_inner);
			if (p_keep_transform) {
				new_inner->set_global_transform(inner->get_global_transform());
			}
		}
		ret.push_back(new_outer);
	}
	return ret;
}

void PolyNode2D::clear() {
	while (get_child_count() > 0) {
		const int idx = get_child_count() - 1;
		PolyNode2D *child = Object::cast_to<PolyNode2D>(get_child(idx));
		if (child) {
			remove_child(child);
			memdelete(child);
		}
	}
	points = Vector<Point2>();
}

void PolyNode2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_points", "points"), &PolyNode2D::set_points);
	ClassDB::bind_method(D_METHOD("get_points"), &PolyNode2D::get_points);
	ClassDB::bind_method(D_METHOD("get_points_transformed"), &PolyNode2D::get_points_transformed);

	ClassDB::bind_method(D_METHOD("set_color", "color"), &PolyNode2D::set_color);
	ClassDB::bind_method(D_METHOD("get_color"), &PolyNode2D::get_color);

	ClassDB::bind_method(D_METHOD("set_open", "open"), &PolyNode2D::set_open);
	ClassDB::bind_method(D_METHOD("is_open"), &PolyNode2D::is_open);

	ClassDB::bind_method(D_METHOD("set_filled", "filled"), &PolyNode2D::set_filled);
	ClassDB::bind_method(D_METHOD("is_filled"), &PolyNode2D::is_filled);

	ClassDB::bind_method(D_METHOD("set_width", "width"), &PolyNode2D::set_width);
	ClassDB::bind_method(D_METHOD("get_width"), &PolyNode2D::get_width);

	ClassDB::bind_method(D_METHOD("new_child", "from_points"), &PolyNode2D::new_child);

	ClassDB::bind_method(D_METHOD("is_hole"), &PolyNode2D::is_hole);
	ClassDB::bind_method(D_METHOD("is_root"), &PolyNode2D::is_root);

	ClassDB::bind_method(D_METHOD("make_from_outlines", "polygons"), &PolyNode2D::make_from_outlines);
	ClassDB::bind_method(D_METHOD("collect_polygons"), &PolyNode2D::collect_polygons);
	ClassDB::bind_method(D_METHOD("collect_polylines"), &PolyNode2D::collect_polylines);
	ClassDB::bind_method(D_METHOD("find_objects"), &PolyNode2D::find_objects);
	ClassDB::bind_method(D_METHOD("separate_objects", "new_parent", "keep_transform"), &PolyNode2D::separate_objects, DEFVAL(Variant()), DEFVAL(true));

	ClassDB::bind_method(D_METHOD("clear"), &PolyNode2D::clear);

	ADD_PROPERTY(PropertyInfo(Variant::POOL_VECTOR2_ARRAY, "points"), "set_points", "get_points");
	ADD_PROPERTY(PropertyInfo(Variant::COLOR, "color"), "set_color", "get_color");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "open"), "set_open", "is_open");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "filled"), "set_filled", "is_filled");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "width"), "set_width", "get_width");
}

PolyNode2D::PolyNode2D() {
	set_notify_local_transform(true);
}

#include "poly_generators_2d.h"
#include "core/math/2d/geometry/goost_geometry_2d.h"

// PolyCircle2D

void PolyCircle2D::set_radius(real_t p_radius) {
	ERR_FAIL_COND(p_radius <= 0);
	radius = p_radius;
	queue_update();
	_change_notify("radius");
}

void PolyCircle2D::set_max_error(real_t p_max_error) {
	ERR_FAIL_COND(p_max_error <= 0);
	max_error = p_max_error;
	queue_update();
	_change_notify("max_error");
}

Vector<Vector<Point2>> PolyCircle2D::_build_outlines() {
	Vector<Vector<Point2>> outlines;
	outlines.push_back(GoostGeometry2D::circle(radius, max_error));
	return outlines;
}

void PolyCircle2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_radius", "radius"), &PolyCircle2D::set_radius);
	ClassDB::bind_method(D_METHOD("get_radius"), &PolyCircle2D::get_radius);

	ClassDB::bind_method(D_METHOD("set_max_error", "max_error"), &PolyCircle2D::set_max_error);
	ClassDB::bind_method(D_METHOD("get_max_error"), &PolyCircle2D::get_max_error);

	ADD_PROPERTY(PropertyInfo(Variant::REAL, "radius", PROPERTY_HINT_RANGE, "0.01,1024.0,0.01,or_greater"), "set_radius", "get_radius");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "max_error", PROPERTY_HINT_RANGE, "0.01,5.0,0.01,or_greater"), "set_max_error", "get_max_error");
}

// PolyRectangle2D

void PolyRectangle2D::set_extents(const Vector2 &p_extents) {
	ERR_FAIL_COND(p_extents.x <= 0);
	ERR_FAIL_COND(p_extents.y <= 0);
	extents = p_extents;
	queue_update();
	_change_notify("extents");
}

Vector<Vector<Point2>> PolyRectangle2D::_build_outlines() {
	Vector<Vector<Point2>> outlines;
	Vector<Point2> rect;
	rect.push_back(Point2(-extents.x, -extents.y));
	rect.push_back(Point2(+extents.x, -extents.y));
	rect.push_back(Point2(+extents.x, +extents.y));
	rect.push_back(Point2(-extents.x, +extents.y));
	outlines.push_back(rect);
	return outlines;
}

void PolyRectangle2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_extents", "extents"), &PolyRectangle2D::set_extents);
	ClassDB::bind_method(D_METHOD("get_extents"), &PolyRectangle2D::get_extents);

	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "extents"), "set_extents", "get_extents");
}

#include "poly_decomp.h"

PolyDecomp2DBackend *PolyDecomp2D::backend = nullptr;

void PolyDecomp2DBackend::set_parameters(const Ref<PolyDecompParameters2D> &p_parameters) {
	if (p_parameters.is_valid()) {
		parameters = p_parameters;
	} else {
		parameters = Ref<PolyDecompParameters2D>();
		parameters = default_parameters;
		parameters->reset();
	}
}

Vector<Vector<Point2>> PolyDecomp2DBackend::decompose_polygons(const Vector<Vector<Point2>> &p_polygons, Decomposition p_type) {
	Vector<Vector<Point2>> polys;
	switch (p_type) {
		case DECOMP_TRIANGLES_EC: {
			polys = triangulate_ec(p_polygons);
		} break;
		case DECOMP_TRIANGLES_OPT: {
			polys = triangulate_opt(p_polygons);
		} break;
		case DECOMP_TRIANGLES_MONO: {
			polys = triangulate_mono(p_polygons);
		} break;
		case DECOMP_CONVEX_HM: {
			polys = decompose_convex_hm(p_polygons);
		} break;
		case DECOMP_CONVEX_OPT: {
			polys = decompose_convex_opt(p_polygons);
		} break;
	}
	return polys;
}

Vector<Vector<Point2>> PolyDecomp2D::triangulate_polygons(const Vector<Vector<Point2>> &p_polygons, const Ref<PolyDecompParameters2D> &p_parameters) {
	backend->set_parameters(p_parameters);
	return backend->triangulate_mono(p_polygons);
}

Vector<Vector<Point2>> PolyDecomp2D::decompose_polygons_into_convex(const Vector<Vector<Point2>> &p_polygons, const Ref<PolyDecompParameters2D> &p_parameters) {
	backend->set_parameters(p_parameters);
	return backend->decompose_convex_hm(p_polygons);
}

Vector<Vector<Point2>> PolyDecomp2D::decompose_polygons(const Vector<Vector<Point2>> &p_polygons, Decomposition p_type, const Ref<PolyDecompParameters2D> &p_parameters) {
	backend->set_parameters(p_parameters);
	return backend->decompose_polygons(p_polygons, PolyDecomp2DBackend::Decomposition(p_type));
}

// BIND

_PolyDecomp2D *_PolyDecomp2D::singleton = nullptr;

void _PolyDecomp2D::set_parameters(const Ref<PolyDecompParameters2D> &p_parameters) {
#ifdef DEBUG_ENABLED
	if (singleton == this) {
		ERR_FAIL_MSG("Configuring parameters is forbidden for a global instance. Please create a new local instance of PolyDecomp2D with `new_instance()` method");
	}
#endif
	parameters = p_parameters;
}

Ref<PolyDecompParameters2D> _PolyDecomp2D::get_parameters() const {
#ifdef DEBUG_ENABLED
	if (singleton == this) {
		ERR_FAIL_V_MSG(Ref<PolyDecompParameters2D>(), "Configuring parameters is forbidden for a global instance. Please create a new local instance of PolyDecomp2D with `new_instance()` method");
	}
#endif
	return parameters;
}

Array _PolyDecomp2D::triangulate_polygons(Array p_polygons) const {
	Vector<Vector<Point2>> polygons;
	for (int i = 0; i < p_polygons.size(); i++) {
		polygons.push_back(p_polygons[i]);
	}
	const auto &params = singleton == this ? Ref<PolyDecompParameters2D>() : parameters;
	Vector<Vector<Vector2>> solution = PolyDecomp2D::triangulate_polygons(polygons, params);
	Array ret;
	for (int i = 0; i < solution.size(); ++i) {
		ret.push_back(solution[i]);
	}
	return ret;
}

Array _PolyDecomp2D::decompose_polygons_into_convex(Array p_polygons) const {
	Vector<Vector<Point2>> polygons;
	for (int i = 0; i < p_polygons.size(); i++) {
		polygons.push_back(p_polygons[i]);
	}
	const auto &params = singleton == this ? Ref<PolyDecompParameters2D>() : parameters;
	Vector<Vector<Vector2>> solution = PolyDecomp2D::decompose_polygons_into_convex(polygons, params);
	Array ret;
	for (int i = 0; i < solution.size(); ++i) {
		ret.push_back(solution[i]);
	}
	return ret;
}

Array _PolyDecomp2D::decompose_polygons(Array p_polygons, Decomposition p_type) const {
	Vector<Vector<Point2>> polygons;
	for (int i = 0; i < p_polygons.size(); i++) {
		polygons.push_back(p_polygons[i]);
	}
	const auto &params = singleton == this ? Ref<PolyDecompParameters2D>() : parameters;
	Vector<Vector<Vector2>> solution = PolyDecomp2D::decompose_polygons(polygons, PolyDecomp2D::Decomposition(p_type), params);
	Array ret;
	for (int i = 0; i < solution.size(); ++i) {
		ret.push_back(solution[i]);
	}
	return ret;
}

void _PolyDecomp2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("new_instance"), &_PolyDecomp2D::new_instance);

	ClassDB::bind_method(D_METHOD("set_parameters", "parameters"), &_PolyDecomp2D::set_parameters);
	ClassDB::bind_method(D_METHOD("get_parameters"), &_PolyDecomp2D::get_parameters);

	ClassDB::bind_method(D_METHOD("triangulate_polygons", "polygons"), &_PolyDecomp2D::triangulate_polygons);
	ClassDB::bind_method(D_METHOD("decompose_polygons_into_convex", "polygons"), &_PolyDecomp2D::decompose_polygons_into_convex);
	ClassDB::bind_method(D_METHOD("decompose_polygons", "polygons", "type"), &_PolyDecomp2D::decompose_polygons);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "parameters"), "set_parameters", "get_parameters");

	BIND_ENUM_CONSTANT(DECOMP_TRIANGLES_EC);
	BIND_ENUM_CONSTANT(DECOMP_TRIANGLES_OPT);
	BIND_ENUM_CONSTANT(DECOMP_TRIANGLES_MONO);
	BIND_ENUM_CONSTANT(DECOMP_CONVEX_HM);
	BIND_ENUM_CONSTANT(DECOMP_CONVEX_OPT);
}

void PolyDecompParameters2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_fill_rule", "fill_rule"), &PolyDecompParameters2D::set_fill_rule);
	ClassDB::bind_method(D_METHOD("get_fill_rule"), &PolyDecompParameters2D::get_fill_rule);

	BIND_ENUM_CONSTANT(FILL_RULE_EVEN_ODD);
	BIND_ENUM_CONSTANT(FILL_RULE_NON_ZERO);
	BIND_ENUM_CONSTANT(FILL_RULE_POSITIVE);
	BIND_ENUM_CONSTANT(FILL_RULE_NEGATIVE);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "fill_rule"), "set_fill_rule", "get_fill_rule");
}

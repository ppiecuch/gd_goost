#include "random.h"

#include "core/method_bind_ext.gen.inc"

Random *Random::singleton = nullptr;

void Random::_bind_methods() {
	ClassDB::bind_method(D_METHOD("new_instance"), &Random::new_instance);

	ClassDB::bind_method(D_METHOD("get_value"), &Random::get_value);
	ClassDB::bind_method(D_METHOD("get_number"), &Random::get_number);
	ClassDB::bind_method(D_METHOD("get_color"), &Random::get_color);
	ClassDB::bind_method(D_METHOD("get_condition"), &Random::get_condition);

	ClassDB::bind_method(D_METHOD("color_hsv", "hue_min", "hue_max", "saturation_min", "saturation_max", "value_min", "value_max", "alpha_min", "alpha_max"),
			&Random::color_hsv, DEFVAL(0.0), DEFVAL(1.0), DEFVAL(0.0), DEFVAL(1.0), DEFVAL(0.0), DEFVAL(1.0), DEFVAL(1.0), DEFVAL(1.0));
	ClassDB::bind_method(D_METHOD("color_rgb", "red_min", "red_max", "green_min", "green_max", "blue_min", "blue_max", "alpha_min", "alpha_max"),
			&Random::color_rgb, DEFVAL(0.0), DEFVAL(1.0), DEFVAL(0.0), DEFVAL(1.0), DEFVAL(0.0), DEFVAL(1.0), DEFVAL(1.0), DEFVAL(1.0));

	ClassDB::bind_method(D_METHOD("range", "from", "to"), &Random::range);
	ClassDB::bind_method(D_METHOD("choice", "from_sequence"), &Random::choice);
	ClassDB::bind_method(D_METHOD("shuffle", "array"), &Random::shuffle);
	ClassDB::bind_method(D_METHOD("decision", "probability"), &Random::decision);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "number"), "", "get_number");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "value"), "", "get_value");
	ADD_PROPERTY(PropertyInfo(Variant::COLOR, "color"), "", "get_color");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "condition"), "", "get_condition");

	// Default values are non-deterministic, override those for documentation purposes.
	ADD_PROPERTY_DEFAULT("number", 37);
	ADD_PROPERTY_DEFAULT("value", 0.5);
	ADD_PROPERTY_DEFAULT("color", Color(0, 0, 1));
	ADD_PROPERTY_DEFAULT("condition", true);
	// Have to override in base class as well.
	ADD_PROPERTY_DEFAULT("seed", 0);
	ADD_PROPERTY_DEFAULT("state", 0);
}

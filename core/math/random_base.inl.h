
uint32_t get_number() { return randi(); }

real_t get_value() { return randf(); }

Color get_color()  {
	Color color;
	// Pick not too pale and not too dark color.
	color.set_hsv(randf(), randf_range(0.5, 1.0), randf_range(0.5, 1.0));
	return color;
}

bool get_condition() { return randf() >= 0.5; }

Variant Random::range(const Variant &p_from, const Variant &p_to) {
	ERR_FAIL_COND_V_MSG(p_from.get_type() != p_to.get_type(), Variant(), "Incompatible types.");

	switch (p_from.get_type()) {
		case Variant::INT: {
			int from = p_from;
			int to = p_to;
			return randi_range(from, to);
		} break;
		case Variant::REAL: {
			real_t from = p_from;
			real_t to = p_to;
			return randf_range(from, to);
		} break;
		default: {
			Variant ret;
			Variant::interpolate(p_from, p_to, randf(), ret);
			return ret;
		}
	}
	return Variant();
}

Color color_hsv(float h_min = 0.0, float h_max = 1.0, float s_min = 0.0, float s_max = 1.0, float v_min = 0.0, float v_max = 1.0, float a_min = 1.0, float a_max = 1.0) {
	Color color;
	color.set_hsv(
			randf_range(h_min, h_max),
			randf_range(s_min, s_max),
			randf_range(v_min, v_max),
			randf_range(a_min, a_max));
	return color;
}

Color color_rgb(float r_min = 0.0, float r_max = 1.0, float g_min = 0.0, float g_max = 1.0, float b_min = 0.0, float b_max = 1.0, float a_min = 1.0, float a_max = 1.0) {
	return Color(
			randf_range(r_min, r_max),
			randf_range(g_min, g_max),
			randf_range(b_min, b_max),
			randf_range(a_min, a_max));
}

Variant range(const Variant &p_from, const Variant &p_to) {
	ERR_FAIL_COND_V_MSG(p_from.get_type() != p_to.get_type(), Variant(), "Incompatible types.");

	switch (p_from.get_type()) {
		case Variant::INT: {
			int from = p_from;
			int to = p_to;
			return randi_range(from, to);
		} break;
		case Variant::REAL: {
			real_t from = p_from;
			real_t to = p_to;
			return randf_range(from, to);
		} break;
		default: {
			Variant ret;
			Variant::interpolate(p_from, p_to, randf(), ret);
			return ret;
		}
	}
	return Variant();
}

Variant choice(const Variant &p_sequence) {
	switch (p_sequence.get_type()) {
		case Variant::STRING: {
			String str = p_sequence;
			ERR_FAIL_COND_V_MSG(str.empty(), Variant(), "String is empty.");
			return str.substr(randi() % str.length(), 1); // Not size().
		} break;
		case Variant::POOL_BYTE_ARRAY:
		case Variant::POOL_INT_ARRAY:
		case Variant::POOL_REAL_ARRAY:
		case Variant::POOL_STRING_ARRAY:
		case Variant::POOL_VECTOR2_ARRAY:
		case Variant::POOL_VECTOR3_ARRAY:
		case Variant::POOL_COLOR_ARRAY:
		case Variant::ARRAY: {
			Array arr = p_sequence;
			ERR_FAIL_COND_V_MSG(arr.empty(), Variant(), "Array is empty.");
			return arr[randi() % arr.size()];
		} break;
		case Variant::DICTIONARY: {
			Dictionary dict = p_from;
			ERR_FAIL_COND_V_MSG(dict.empty(), Variant(), "Dictionary is empty.");
			return dict.get_value_at_index(randi() % dict.size());
		} break;
		default: {
			ERR_FAIL_V_MSG(Variant(), "Unsupported: the type must be indexable.");
		}
	}
	return Variant();
}

void shuffle(Array p_array) {
	if (p_array.size() < 2) {
		return;
	}
	for (int i = p_array.size() - 1; i > 0; --i) {
		const uint32_t j = randi() % (i + 1);
		const Variant tmp = p_array[i];
		p_array[i] = p_array[j];
		p_array[j] = tmp;
	}
}

bool decision(float probability) { return randf() <= probability; }

#pragma once

#include "core/math/random_number_generator.h"

class Random : public RandomNumberGenerator {
	GDCLASS(Random, RandomNumberGenerator);

private:
	static Random *singleton;

protected:
	static void _bind_methods();

public:
	static Random *get_singleton() { return singleton; }
	virtual Ref<Reference> new_instance() const { return memnew(Random); }

#	include "random_base.inl.h"

	Random() {
		if (!singleton) {
			randomize(); // Only the global one is randomized automatically.
			singleton = this;
		}
	}
};


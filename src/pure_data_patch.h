#pragma once

#include <godot_cpp/classes/ref_counted.hpp>

namespace godot {

class PureDataPatch : public RefCounted {
	GDCLASS(PureDataPatch, RefCounted)

	void* handle_{};

protected:
	static void _bind_methods();

public:
	PureDataPatch() = default;
	~PureDataPatch() = default;

	bool open(String path);
	void close();

};

};

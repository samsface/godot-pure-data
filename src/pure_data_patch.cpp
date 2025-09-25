#include "pure_data_patch.h"
#include <godot_cpp/core/class_db.hpp>

#define PD_INTERNAL

#include "z_libpd.h"

using namespace godot;

void PureDataPatch::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("open"), &PureDataPatch::open);
	ClassDB::bind_method(D_METHOD("close"), &PureDataPatch::close);
}

bool PureDataPatch::open(String path)
{
	handle_ = ::libpd_openfile(path.get_file().utf8().get_data(), path.get_base_dir().utf8().get_data());
	return handle_ != nullptr;
}

void PureDataPatch::close()
{
	::libpd_closefile(handle_);
}

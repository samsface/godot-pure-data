class_name PureDataPatchFile extends Resource

## The file path to the patch within the Pure Data directory structure.
## This is used when loading patches as temporary files, so it needs to be unique and should match up with paths to abstractions.
@export var file_path: String
## The data contained within the patch file.
@export var patch_data: PackedByteArray

## Directory to store temporary patch files in.
const TEMP_DIR = "user://.godot-pure-data-tmp"

## Path where this patch has temporary been stored.
var temp_path


## Creates a temporary patch file if one doesn't already exist and returns its path.
func get_temp_path() -> String:
	var err: Error
	
	# Figure out where the temporary file will go
	temp_path = TEMP_DIR.path_join(file_path)
	
	# Make sure that the temporary directory exists
	var user_dir = DirAccess.open("user://")
	err = DirAccess.get_open_error()
	if err != OK:
		push_error("Failed to access user directory: ", error_string(err))
		return ""
	
	if not user_dir.dir_exists(temp_path.get_base_dir()):
		err = user_dir.make_dir(temp_path.get_base_dir())
	if err != OK:
		push_error("Failed to create temporary directory: ", error_string(err))
		return ""
	
	# Write a temporary file to the disk
	var save_file := FileAccess.open(temp_path, FileAccess.WRITE)
	err = FileAccess.get_open_error()
	if err != OK:
		push_error("Failed to save temporary patch file: ", error_string(err))
		return ""
	
	save_file.store_buffer(patch_data)
	
	return temp_path


## Creates a temporary patch file if one doesn't already exist and returns a global path to it.
func get_global_path() -> String:
	return ProjectSettings.globalize_path(get_temp_path())

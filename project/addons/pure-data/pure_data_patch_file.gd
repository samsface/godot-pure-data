class_name PureDataPatchFile extends Resource

## Path to the patch within the project.
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
	
	# Make sure that the temporary directory exists
	var user_dir = DirAccess.open("user://")
	err = DirAccess.get_open_error()
	if err != OK:
		push_error("Failed to access user directory!")
		return ""
	
	if not user_dir.dir_exists(TEMP_DIR):
		err = user_dir.make_dir(TEMP_DIR)
	if err != OK:
		push_error("Failed to create temporary directory!")
		return ""
	
	# Write a temporary file to the disk
	temp_path = "%s/%s" % [TEMP_DIR, file_path.get_file()]
	var save_file := FileAccess.open(temp_path, FileAccess.WRITE)
	err = FileAccess.get_open_error()
	if err != OK:
		push_error("Failed to save temporary patch file!")
		return ""
	
	save_file.store_buffer(patch_data)
	
	return temp_path


## Creates a temporary patch file if one doesn't already exist and returns a global path to it.
func get_global_path() -> String:
	return ProjectSettings.globalize_path(get_temp_path())

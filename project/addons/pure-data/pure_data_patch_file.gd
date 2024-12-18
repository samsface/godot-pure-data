class_name PureDataPatchFile extends Resource

@export var file_path: String


func get_global_path() -> String:
	return ProjectSettings.globalize_path(file_path)

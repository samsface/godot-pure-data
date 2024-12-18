class_name PureDataPatchInstance extends Node

@export var patch_file: PureDataPatchFile

var patch_handle = PureDataPatch.new()


func _ready() -> void:
	if not patch_handle.open(patch_file.get_global_path()):
		push_error("Couldn't open %s!" % patch_file.get_global_path())

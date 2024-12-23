class_name PureDataPatchInstance extends Node

@export var patch_file: PureDataPatchFile

var patch_handle = PureDataPatch.new()


func _ready() -> void:
	# Check for any child abstractions and make sure they are loaded.
	for child in get_children():
		if child is PureDataAbstraction:
			if is_instance_valid(child.patch_file):
				child.patch_file.create_temporary_file()
	
	# Load the patch
	if not patch_handle.open(patch_file.get_global_path()):
		push_error("Couldn't open %s!" % patch_file.get_global_path())

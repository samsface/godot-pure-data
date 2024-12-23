@tool
extends EditorPlugin

var pure_data_patch_import_plugin


func _enter_tree():
	add_custom_type("PureDataPatchFile", "Resource", preload("pure_data_patch_file.gd"), preload("icon.png"))
	pure_data_patch_import_plugin = preload("pure_data_patch_import_plugin.gd").new()
	add_import_plugin(pure_data_patch_import_plugin)


func _exit_tree():
	remove_import_plugin(pure_data_patch_import_plugin)
	pure_data_patch_import_plugin = null

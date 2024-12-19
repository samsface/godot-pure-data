@tool
extends EditorImportPlugin

## The file prefixes that will be trimmed off of the file path on import.
const FILE_PREFIXES = ["res://", "user://"]


func _get_importer_name():
	return "pure_data.patch"


func _get_visible_name():
	return "Pure Data Patch"


func _get_recognized_extensions():
	return ["pd"]


func _get_save_extension():
	return "tres"


func _get_resource_type():
	return "PureDataPatchFile"


func _get_preset_count():
	return 0


func _get_preset_name(preset_index):
	return ""


func _get_import_options(path, preset_index):
	return []


func _get_option_visibility(path, option_name, options):
	return false


func _get_import_order():
	return ResourceImporter.ImportOrder.IMPORT_ORDER_DEFAULT


func _get_priority():
	return 1


func _import(source_file, save_path, options, r_platform_variants, r_gen_files):
	var err: Error
	
	# Load the data from the patch file
	var patch_data := FileAccess.get_file_as_bytes(source_file)
	err = FileAccess.get_open_error()
	if err != OK:
		return err
	
	# Simplify the filepath and trim the prefix off if it has one.
	var file_path = source_file.simplify_path()
	for prefix in FILE_PREFIXES:
		file_path = file_path.trim_prefix(prefix)
	
	# Create a PureDataPatchFile and save the patch data to it
	var patch_file := PureDataPatchFile.new()
	patch_file.file_path = file_path
	patch_file.patch_data = patch_data
	
	# Save the PureDataPatchFile to disk
	var resource_file_name := "%s.%s" % [save_path, _get_save_extension()]
	return ResourceSaver.save(patch_file, resource_file_name)

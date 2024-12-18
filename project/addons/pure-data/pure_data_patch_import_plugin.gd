@tool
extends EditorImportPlugin


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
	var patch_file := PureDataPatchFile.new()
	patch_file.file_path = source_file
	
	var file_name := "%s.%s" % [save_path, _get_save_extension()]
	return ResourceSaver.save(patch_file, file_name)

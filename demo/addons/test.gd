extends Node

#var patch_file_handle_ := PDPatchFile.new()

func _ready():
	var p = ProjectSettings.globalize_path("res://patch.pd")
	
	#if not patch_file_handle_.open(p):
	#	push_error("couldn't open patch")
		
	#$GDExample.play()

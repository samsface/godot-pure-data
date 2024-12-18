extends Node2D

var patch_file_handle_ = PureDataPatch.new()
@export var patch: PureDataPatchFile


# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	$PureDataAudioStreamPlayer.play()
	#$PureDataAudioStreamPlayer.start_gui("/Applications/Pd-0.54-1.app/Contents/MacOS")


func _on_h_slider_value_changed(value: float) -> void:
	$PureDataAudioStreamPlayer.send_float("osc-frequency", value)

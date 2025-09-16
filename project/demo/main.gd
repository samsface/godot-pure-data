extends Control

var patch_file_handle_ = PureDataPatch.new()
@export var patch: PureDataPatchFile


# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	$AudioStreamPlayer.play()
	#$AudioStreamPlayer.stream.start_gui("/Applications/Pd-0.55-2.app/Contents/MacOS")


func _on_frequency_slider_value_changed(value: float) -> void:
	$AudioStreamPlayer.stream.send_float("osc-frequency", value)


func _on_harmonic_slider_value_changed(value: float) -> void:
	$AudioStreamPlayer.stream.send_float("harmonic-ratio", value)


func _on_modulation_slider_value_changed(value: float) -> void:
	$AudioStreamPlayer.stream.send_float("modulation-index", value)

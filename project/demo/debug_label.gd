extends Label

@export var patch: PureDataPatchFile

func _ready() -> void:
	text = patch.get_global_path()

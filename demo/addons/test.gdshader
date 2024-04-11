shader_type spatial;

uniform sampler2D noise;
uniform sampler2D climate;

varying float y;

void vertex() {
	y = (VERTEX.y + 0.5);
	// Called for every vertex the material is visible on.
}

void fragment() {
	ALBEDO = texture(noise, UV).rgb;
	ALBEDO *= texture(climate, vec2(y)).rgb;
	
	if(length(ALBEDO) <= 0.1)
	{
		ALBEDO = vec3(0.0, 0.0, 100.0);
		METALLIC = 1.0;
		SPECULAR = 1.0;
		ROUGHNESS = 0.1;
	}
	
	
	EMISSION = vec3(0.0, 0.0, 1.0) * 0.1;
	

	
	// Called for every pixel the material is visible on.
}

//void light() {
	// Called for every pixel for every light affecting the material.
	// Uncomment to replace the default light processing function with this one.
//}

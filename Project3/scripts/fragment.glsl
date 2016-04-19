precision mediump float;

uniform sampler2D uSampler;
uniform vec4 uColor;

uniform vec3 uPositionLight;
uniform vec3 uPositionColor;
uniform vec3 uAmbientColor;

varying vec2 vTexCoord;
varying vec3 vNormal;
varying vec4 vPosition;

void main(void) {
	vec3 vLightWeight = vec3(1.0, 1.0, 1.0);
	vec3 vLightDirection = vec3(1.0, 1.0, 1.0);

	if((uColor.x == 0.0) && (uColor.y == 0.0) && (uColor.z == 0.0) && (uColor.w == 0.0)) {
		vLightDirection = normalize(uPositionLight - vPosition.xyz);
		float directionalLightWeighting = max(dot(normalize(vNormal), vLightDirection), 0.0);
		vLightWeight = uAmbientColor + uPositionColor * directionalLightWeighting;

		vec4 fragColor = texture2D(uSampler, vec2(vTexCoord.s, vTexCoord.t));

		gl_FragColor = vec4(fragColor.rgb * vLightWeight, fragColor.a);

		//gl_FragColor = texture2D(uSampler, vec2(vTexCoord.s, vTexCoord.t));
	}
	else {
		gl_FragColor = uColor;
	}
}

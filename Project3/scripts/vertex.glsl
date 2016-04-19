attribute vec3 aPosition;
attribute vec3 aNormal;
attribute vec2 aTexCoord;

uniform mat4 uPMatrix;
uniform mat4 uMVMatrix;
uniform mat3 uNMatrix;

varying vec2 vTexCoord;
varying vec3 vNormal;
varying vec4 vPosition;

void main(void) {
	vPosition 	= uMVMatrix * vec4(aPosition, 1.0);
	vNormal 		= uNMatrix * aNormal;
	vTexCoord 	= aTexCoord;

	gl_Position 	= uPMatrix * vPosition;
}

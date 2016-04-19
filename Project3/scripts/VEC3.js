var Vec3 = function() {}

Vec3.create = function(a, b, c) {
	return new Float32Array([a, b, c]);
}

Vec3.add = function(a, b, out) {
	out[0] = a[0] + b[0];
	out[1] = a[1] + b[1];
	out[2] = a[2] + b[2];
}

Vec3.sub = function(a, b, out) {
	out[0] = a[0] - b[0];
	out[1] = a[1] - b[1];
	out[2] = a[2] - b[2];
}

Vec3.scale = function(a, v, out) {
	out[0] = a[0] * v;
	out[1] = a[1] * v;
	out[2] = a[2] * v;
}

Vec3.length = function(a) {
	return Math.sqrt((a[0]*a[0]) + (a[1]*a[1]) + (a[2]*a[2]));
}

Vec3.dot = function(a, b) {
	return ((a[0]*b[0]) + (a[1]*b[1]) + (a[2]*b[2]));
}

Vec3.normalize = function(a, out) {
	var iLen = 1 / Vec3.length(a);
	out[0] = a[0] * iLen;
	out[1] = a[1] * iLen;
	out[2] = a[2] * iLen;
}

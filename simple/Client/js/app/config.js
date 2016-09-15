class MuskatConfig {
	constructor(callback) {
		// render resolution
		this._width		= 512;
		this._height 	= 512;
		
		// texture compression params
		this._textureCompressionMethod 	= "jpeg"; // jpeg or png
		this._textureCompressionQuality = 0;	  // value (0 - 100), (1 - 9)

		// mesh modies
		// full represent a fully connected mesh and
		// delaunay for a delaunay constructed mesh
		this._mesh_mode	= "full";

		// grid type is a parameter for full connected meshs
		this._grid_type = "default";

		// 8 or 16 bit values (full connected)
		this._mesh_percesion = "8bit";

		// PNG compression for full connected meshs
		this._mesh_compression	= 1;

		this._max_depth		= 1;	// max depth of quadtree
		this._T_leaf 		= 0;	// thresholds needed to generate seeds
		this._T_internal 	= 0;	// - " -
		
		// callback function, for example sending the config
		this._callback = callback;
	}

	get textureCompressionMethod() {
		return this._textureCompressionMethod;
	}

	get textureCompressionQuality() {
		return this._textureCompressionQuality;
	}

	get width() {
		return this._width;
	}

	get height() {
		return this._height;
	}

	get meshMode() {
		return this._mesh_mode;
	}

	get gridType() {
		return this._grid_type;
	}

	get meshPercesion() {
		return this._mesh_percesion;
	}

	get meshCompression() {
		return this._mesh_compression;
	}

	get maxDepth() {
		return this._max_depth;
	}

	get Tleaf() {
		return this._T_leaf;
	}

	get Tinternal() {
		return this._T_internal;
	}

	get meshState() {
		if(this._mesh_mode == "delaunay") {
			return 2;
		} else if(this._mesh_mode == "full") {
			if(this._mesh_percesion == "8bit") {
				return 0;
			} else {
				return 1;
			}
		}
	}

	set textureCompressionMethod(method) {
		this._textureCompressionMethod = method;
		this._callback();
	}

	set textureCompressionQuality(quality) {
		this._textureCompressionQuality = quality;
		this._callback();
	}

	set width(w) {
		this._width = w;
		this._callback();
	}

	set height(h) {
		this._height = h;
		this._callback();
	}

	set meshMode(mode) {
		this._mesh_mode = mode;
		this._callback();
	}

	set gridType(type) {
		this._grid_type = type;
		this._callback();
	}

	set meshPercesion(percesion) {
		this._mesh_percesion = percesion;
		this._callback();
	}

	set meshCompression(compression) {
		this._mesh_compression = compression;
		this._callback();
	}

	set maxDepth(depth) {
		this._max_depth = depth;
		this._callback();
	}

	set Tleaf(T) {
		this._T_leaf = T;
		this._callback();
	}

	set Tinternal(T) {
		this._T_internal = T;
		this._callback();
	}
}

class MuskatConfig {
	constructor(callback) {
		// render resolution
		this._width		= 512;
		this._height 	= 512;

		this._mesh_width	= 512;
		this._mesh_height	= 512;

		this._smooth_depth	= false;
		
		// texture compression params
		this._textureCompressionMethod 	= "png"; // jpeg or png
		this._textureCompressionQuality = 0;	  // value (0 - 100), (1 - 9)

		// mesh modies
		// full represent a fully connected mesh and
		// delaunay for a delaunay constructed mesh
		this._mesh_mode	= "full";

		this._seed_mode = "quadtree";

		// grid type is a parameter for full connected meshs
		this._grid_type = "default";

		// 8 or 16 bit values (full connected)
		this._mesh_precision = "8bit";

		// PNG compression for full connected meshs
		this._mesh_compression	= 9;

		this._max_depth		= 8;	// max depth of quadtree
		this._T_leaf 		= 0;	// thresholds needed to generate seeds
		this._T_internal 	= 0;	// - " -
		this._T_angle		= 0.2;
		this._T_join		= 0.5;
		this._T_grad		= 1.0;

		this._T_threshold	= 0.5;
		this._gamma			= 0.5;

		this._five_pass		= true;

		this._refine		= false;
		
		this._pre_background_subtraction = false;
		this._pra_background_subtraction = false;

		// callback function, for example sending the config
		this._callback = callback;
	}

	setDefaults() {
		this._width		= 512;
		this._height 	= 512;

		this._mesh_width	= 512;
		this._mesh_height	= 512;

		this._smooth_depth	= false;
		
		// texture compression params
		this._textureCompressionMethod 	= "png"; // jpeg or png
		this._textureCompressionQuality = 0;	  // value (0 - 100), (1 - 9)

		// mesh modies
		// full represent a fully connected mesh and
		// delaunay for a delaunay constructed mesh
		this._mesh_mode	= "full";

		this._seed_mode = "quadtree";

		// grid type is a parameter for full connected meshs
		this._grid_type = "default";

		// 8 or 16 bit values (full connected)
		this._mesh_precision = "8bit";

		// PNG compression for full connected meshs
		this._mesh_compression	= 0;

		this._max_depth		= 8;	// max depth of quadtree
		this._T_leaf 		= 0;	// thresholds needed to generate seeds
		this._T_internal 	= 0;	// - " -
		this._T_angle		= 0.2;
		this._T_join		= 0.5;
		this._T_grad		= 1.0;

		this._T_threshold	= 0.5;
		this._gamma			= 0.5;

		this._five_pass		= true;
		
		this._refine		= true;
		
		this._pre_background_subtraction = false;
		this._pra_background_subtraction = false;
		this._callback();
	}

	set16Bit() {
		meshPrecision = "16bit";
	}

	get name() {
		var name = this.meshWidth+"x"+this.meshHeight+"_";

		if(this.meshMode == "full") {
			name += "Full/"+this.gridType+"/"+this.meshPrecision;
			
			if(this.fivePass) 	name += "/5Pass";
			else		 		name += "/3Pass";
			
			if(this.smoothDepth) name += "_smooth";

			name += "/G"+ this.Tgrad.toFixed(1);
		} else {
			name += "Delaunay/";
	
			if(this.seedMode == "quadtree") name += "D"+this.maxDepth+"/L"+this.Tleaf.toFixed(1)+"/I"+this.Tinternal.toFixed(1);
			else							name += "T"+this.Tthreshold.toFixed(1)+"/G"+this.gamma.toFixed(1);

			if(this.refine) name += "/A"+ this.Tangle.toFixed(1) +"/J"+ this.Tjoin.toFixed(1);

			if(this.preBackgroundSubtraction || this.praBackgroundSubtraction) name += "/";
			if(this.preBackgroundSubtraction) name += "_pre";
			if(this.praBackgroundSubtraction) name += "_pra";
		}

		return name;
	}

	get shortName() {
		var name = this.meshWidth+"x"+this.meshHeight;

		if(this.meshMode == "full") {
			name += "F";
			if(this.gridType == "default") name += "d";
			else if(this.gridType == "cookie_cutter") name += "c";
			else name += "i";
			
			if(this.meshPrecision == "8bit") name += "8";
			else name += "16";
			
			if(this.fivePass) 	name += "5";
			else		 		name += "3";
			
			name += this.Tgrad.toFixed(1);
			
			if(this.smoothDepth) name += "s";
		} else {
			if(this.seedMode == "quadtree") name += "D"+this.maxDepth+"L"+this.Tleaf.toFixed(1)+"I"+this.Tinternal.toFixed(1);
			else							name += "T"+this.Tthreshold.toFixed(1)+"G"+this.gamma.toFixed(1);
	
			if(this.refine) name += "A"+ this.Tangle.toFixed(1) +"J"+ this.Tjoin.toFixed(1);

			if(this.preBackgroundSubtraction) name += "pre";
			if(this.praBackgroundSubtraction) name += "pra";
		}

		return name;
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

	get meshWidth() {
		return this._mesh_width;
	}

	get meshHeight() {
		return this._mesh_height;
	}

	get smoothDepth() {
		return this._smooth_depth;
	}

	get meshMode() {
		return this._mesh_mode;
	}

	get seedMode() {
		return this._seed_mode;
	}

	get gridType() {
		return this._grid_type;
	}

	get meshPrecision() {
		return this._mesh_precision;
	}

	get fivePass() {
		return this._five_pass;
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

	get Tthreshold() {
		return this._T_threshold;
	}

	get gamma() {
		return this._gamma;
	}

	get Tangle() {
		return this._T_angle;
	}

	get Tjoin() {
		return this._T_join;
	}

	get Tgrad() {
		return this._T_grad;
	}

	get meshState() {
		if(this._mesh_mode == "delaunay") {
			return 2;
		} else if(this._mesh_mode == "full") {
			if(this._mesh_precision == "8bit") {
				return 0;
			} else {
				return 1;
			}
		}
	}

	get refine() {
		return this._refine;
	}

	get preBackgroundSubtraction() {
		return this._pre_background_subtraction;
	}

	get praBackgroundSubtraction() {
		return this._pra_background_subtraction;
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
	//	this._callback();
	}

	set height(h) {
		this._height = h;
		this._callback();
	}

	set meshWidth(w) {
		this._mesh_width = w;
	//	this._callback();
	}

	set meshHeight(h) {
		this._mesh_height = h;
		this._callback();
	}

	set smoothDepth(value) {
		this._smooth_depth = value;
		this._callback();
	}

	set meshMode(mode) {
		this._mesh_mode = mode;
		this._callback();
	}

	set seedMode(mode) {
		this._seed_mode = mode;
		this._callback();
	}

	set gridType(type) {
		this._grid_type = type;
		this._callback();
	}

	set meshPrecision(precision) {
		this._mesh_precision = precision;
		this._callback();
	}

	set fivePass(value) {
		this._five_pass = value;
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

	set Tangle(T) {
		this._T_angle = T;
		this._callback();
	}

	set Tjoin(T) {
		this._T_join = T;
		this._callback();
	}

	set Tgrad(T) {
		this._T_grad = T;
		this._callback();
	}

	set Tthreshold(T) {
		this._T_threshold = T;
		this._callback();
	}

	set gamma(g) {
		this._gamma = g;
		this._callback();
	}
 
	set refine(value) {
		this._refine = value;
		this._callback();
	}

	set preBackgroundSubtraction(value) {
		this._pre_background_subtraction = value;
		this._callback();
	}

	set praBackgroundSubtraction(value) {
		this._pra_background_subtraction = value;
		this._callback();
	}
}

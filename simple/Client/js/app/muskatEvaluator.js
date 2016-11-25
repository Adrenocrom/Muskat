class MuskatEvaluator {
	constructor() {
		this.setDefault();
	}

	setDefault() {
		this._evaluation = [];

		// Full Mesh
		var gridTypes 		= ["default", "cookie_cutter", "isometric"];
		var meshPrecision 	= ["16bit", "8bit"];
		var gradients 		= [1.0, 0.9, 0.8];
		var angles			= [0.1, 1, 10];
		for(var m = 1; m <= 1; m += m) {
			for(var p = 0; p < meshPrecision.length; p++) {
				for(var g = 0; g < gradients.length; g++) {
					var c1 = new MuskatConfig(this.nullFunction);
					c1.meshWidth 		= c1.width 	/ m;
					c1.meshHeight 		= c1.height / m;
					c1.gridType			= gridTypes[0];
					c1.meshPrecision	= meshPrecision[p];
					c1.Tgrad 			= gradients[g];
					c1.fivePass			= true;
					this.add(c1);
					var c2 = new MuskatConfig(this.nullFunction);
					c2.meshWidth 		= c2.width 	/ m;
					c2.meshHeight 		= c2.height / m;
					c2.gridType			= gridTypes[0];
					c2.meshPrecision	= meshPrecision[p];
					c2.Tgrad 			= gradients[g];
					c2.fivePass			= false;
					this.add(c2);
				}
			}
		}
		
		for(var t = 0.0; t <= 1.0; t+= 0.1) {
			for(var g = 0.0; g <= 1.0; g+= 0.1) {
				var f7 = new MuskatConfig(this.nullFunction);
				f7.meshMode = "delaunay";
				f7.seedMode = "floyd_steinberg";
				f7.Tthreshold = t;
				f7.gamma	  = g;
				f7.refine	  = false;
				f7.preBackgroundSubtraction = false;
				f7.praBackgroundSubtraction = false;
				this.add(f7);

				if(g >= 0.8 && t >= 0.8) {
					for(var a = 0; a < angles.length; a++) {
						for(var j = 0.1; j <= 0.2; j += 0.1) {
							var f13 = new MuskatConfig(this.nullFunction);
							f13.meshMode = "delaunay";
							f13.seedMode = "floyd_steinberg";
							f13.Tthreshold = t;
							f13.gamma	 = g;
							f13.refine	 = true;
							f13.Tangle	 = angles[a];
							f13.Tjoin	 = j;
							f13.preBackgroundSubtraction = false;
							f13.praBackgroundSubtraction = false;
							this.add(f13);
						}
					}
				}
			}
		}

		// Delaunay Mesh QTree
		for(var d = 8; d <= 10; d++) {
			for(var l = 0.0; l <= 1.0; l += 0.1) {
				for(var i = 0.0; i <= l; i += 0.1) {							
					var c9 = new MuskatConfig(this.nullFunction);
					c9.meshMode  = "delaunay";
					c9.maxDepth  = d;
					c9.Tleaf	 = l;
					c9.Tinternal = i;
					c9.refine	 = false;
					c9.preBackgroundSubtraction = true;
					c9.praBackgroundSubtraction = false;
					this.add(c9);
				
					if(l >= 0.4 && i >= 0.4) {
						for(var a = 0; a < angles.length; a++) {
							for(var j = 0.1; j <= 0.2; j += 0.1) {
								var d2 = new MuskatConfig(this.nullFunction);
								d2.meshMode  = "delaunay";
								d2.maxDepth  = d;
								d2.Tleaf	 = l;
								d2.Tinternal = i;
								d2.refine	 = true;
								d2.Tangle	 = angles[a];
								d2.Tjoin	 = j;
								d2.preBackgroundSubtraction = true;
								d2.praBackgroundSubtraction = false;
								this.add(d2);
							}
						}
					}
				}
			}
		}
	}

	add(config) {
		this._evaluation.push(config);
	}

	setFromConfig(config) {
		this._evaluation = [];
		this.add(config);
	}

	getConfig(index) {
		return this._evaluation[index];
	}

	get length() {
		return this._evaluation.length;
	}

	get evaluation() {
		return this._evaluation;
	}

	nullFunction() {}
}

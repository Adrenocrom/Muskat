class MuskatEvaluator {
	constructor() {
		this._evaluation = [];
	
		// Full Mesh
		var gridTypes = ["default", "cookie_cutter", "isometric"];
		var meshPercision = ["8bit", "16bit"];
		for(var m = 1; m <= 4; m += m) {
			for(var g = 0; g < gridTypes.length; g++) {
				for(var p = 0; p < meshPercision.length; p++) {
					for(var i = 0.0; i <= 1.0; i += 0.1) {
						var c1 = new MuskatConfig(this.nullFunction);
						c1.meshWidth 		= c1.width 	/ m;
						c1.meshHeight 		= c1.height / m;
						c1.gridType			= gridTypes[g];
						c1.meshPercision	= meshPercision[p];
						c1.Tgrad 			= i;
						c1.fivePass			= true;
						var c2 = new MuskatConfig(this.nullFunction);
						c2.meshWidth 		= c2.width 	/ m;
						c2.meshHeight 		= c2.height / m;
						c2.gridType			= gridTypes[g];
						c2.meshPercision	= meshPercision[p];
						c2.Tgrad 			= i;
						c2.fivePass			= false;

						this.add(c1);
						this.add(c2);
					}
				}
			}

			var config = new MuskatConfig(this.nullFunction);
			config.mesh_mode = "delaunay";
			
			var max_depth = 8;
			if(m == 1) max_depth = 10;
			if(m == 2) max_depth = 9;
			if(m == 4) max_depth = 8;

			for(var d = 1; d <= max_depth; d++) {
				for(var l = 0.0; l <= 1.0; l += 0.1) {
					for(var i = 0.0; i <= l; i += 0.1) {
						for(var a = 0.0; a <= 200.0; a += 20.0) {
							for(var j = 0.0; j <= 2.0; j += 1.0) {
								var c3 = new MuskatConfig(this.nullFunction);
								c3.mesh_mode = "delaunay";
								c3.max_depth = d;
								c3.Tleaf	 = l;
								c3.Tinternal = i;
								c3.refine	 = true;
								c3.Tangle	 = a;
								c3.Tjoin	 = j;
								c3.preBackgroundSubtraction = false;
								c3.praBackgroundSubtraction = false;
								this.add(c3);

								var c4 = new MuskatConfig(this.nullFunction);
								c4.mesh_mode = "delaunay";
								c4.max_depth = d;
								c4.Tleaf	 = l;
								c4.Tinternal = i;
								c4.refine	 = true;
								c4.Tangle	 = a;
								c4.Tjoin	 = j;
								c4.preBackgroundSubtraction = false;
								c4.praBackgroundSubtraction = true;
								this.add(c4);
								
								var c5 = new MuskatConfig(this.nullFunction);
								c5.mesh_mode = "delaunay";
								c5.max_depth = d;
								c5.Tleaf	 = l;
								c5.Tinternal = i;
								c5.refine	 = true;
								c5.Tangle	 = a;
								c5.Tjoin	 = j;
								c5.preBackgroundSubtraction = true;
								c5.praBackgroundSubtraction = false;
								this.add(c5);

								var c6 = new MuskatConfig(this.nullFunction);
								c6.mesh_mode = "delaunay";
								c6.max_depth = d;
								c6.Tleaf	 = l;
								c6.Tinternal = i;
								c6.refine	 = true;
								c6.Tangle	 = a;
								c6.Tjoin	 = j;
								c6.preBackgroundSubtraction = true;
								c6.praBackgroundSubtraction = true;
								this.add(c6);
							}
						}

						var c7 = new MuskatConfig(this.nullFunction);
						c7.mesh_mode = "delaunay";
						c7.max_depth = d;
						c7.Tleaf	 = l;
						c7.Tinternal = i;
						c7.refine	 = false;
						c7.preBackgroundSubtraction = false;
						c7.praBackgroundSubtraction = false;
						this.add(c7);

						var c8 = new MuskatConfig(this.nullFunction);
						c8.mesh_mode = "delaunay";
						c8.max_depth = d;
						c8.Tleaf	 = l;
						c8.Tinternal = i;
						c8.refine	 = false;
						c8.preBackgroundSubtraction = false;
						c8.praBackgroundSubtraction = true;
						this.add(c8);
								
						var c9 = new MuskatConfig(this.nullFunction);
						c9.mesh_mode = "delaunay";
						c9.max_depth = d;
						c9.Tleaf	 = l;
						c9.Tinternal = i;
						c9.refine	 = false;
						c9.preBackgroundSubtraction = true;
						c9.praBackgroundSubtraction = false;
						this.add(c9);

						var c10 = new MuskatConfig(this.nullFunction);
						c10.mesh_mode = "delaunay";
						c10.max_depth = d;
						c10.Tleaf	 = l;
						c10.Tinternal = i;
						c10.refine	 = false;
						c10.preBackgroundSubtraction = true;
						c10.praBackgroundSubtraction = true;
						this.add(c10);
					}
				}
			}
		}
	}

	add(config) {
		this._evaluation.push(config);
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

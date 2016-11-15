class MuskatEvaluator {
	constructor() {
		this._evaluation = [];
	
		// Full Mesh
		var gridTypes 		= ["default", "cookie_cutter", "isometric"];
		var meshPrecision 	= ["16bit", "8bit"];
		var gradients 		= [1.0, 0.9, 0.8, 0.7, 0.6, 0.5, 0.4, 0.3, 0.2, 0.1];
		for(var m = 1; m <= 4; m += m) {
			for(var p = 0; p < meshPrecision.length; p++) {
				for(var g = 0; g < gradients.length; g++) {
					var c1 = new MuskatConfig(this.nullFunction);
					c1.meshWidth 		= c1.width 	/ m;
					c1.meshHeight 		= c1.height / m;
					c1.gridType			= gridTypes[0];
					c1.meshPrecision	= meshPrecision[p];
					c1.Tgrad 			= gradients[g];
					c1.fivePass			= true;
					var c2 = new MuskatConfig(this.nullFunction);
					c2.meshWidth 		= c2.width 	/ m;
					c2.meshHeight 		= c2.height / m;
					c2.gridType			= gridTypes[0];
					c2.meshPrecision	= meshPrecision[p];
					c2.Tgrad 			= gradients[g];
					c2.fivePass			= false;
					this.add(c1);
					this.add(c2);
				}
			}
		}
		
		// Delaunay Mesh floyd-steinberg
		var yyy = new MuskatConfig(this.nullFunction);
		yyy.meshMode = "delaunay";
		yyy.seedMode = "floyd_steinberg";
		yyy.refine	 = false;
		yyy.preBackgroundSubtraction = false;
		yyy.praBackgroundSubtraction = false;
		this.add(yyy);

		for(var t = 0.0; t <= 1.0; t+=1.0) {
			for(var g = 0.0; g <= 1.0; g+=1.0) {
				var f7 = new MuskatConfig(this.nullFunction);
				f7.meshMode = "delaunay";
				f7.seedMode = "floyd_steinberg";
				f7.Tthreshold = t;
				f7.gamma	 = g;
				f7.refine	 = false;
				f7.preBackgroundSubtraction = false;
				f7.praBackgroundSubtraction = false;
				this.add(f7);

				var f8 = new MuskatConfig(this.nullFunction);
				f8.meshMode = "delaunay";
				f8.seedMode = "floyd_steinberg";
				f8.Tthreshold = t;
				f8.gamma	 = g;
				f8.refine	 = false;
				f8.preBackgroundSubtraction = false;
				f8.praBackgroundSubtraction = true;
				this.add(f8);
						
				var f9 = new MuskatConfig(this.nullFunction);
				f9.meshMode = "delaunay";
				f9.seedMode = "floyd_steinberg";
				f9.Tthreshold = t;
				f9.gamma	 = g;
				f9.refine	 = false;
				f9.preBackgroundSubtraction = true;
				f9.praBackgroundSubtraction = false;
				this.add(f9);

				var f10 = new MuskatConfig(this.nullFunction);
				f10.meshMode = "delaunay";
				f10.seedMode = "floyd_steinberg";
				f10.Tthreshold = t;
				f10.gamma	 = g;
				f10.refine	 = false;
				f10.preBackgroundSubtraction = true;
				f10.praBackgroundSubtraction = true;
				this.add(f10);

				for(var a = 0.0; a <= 160.0; a += 20.0) {
					for(var j = 2.0; j <= 2.0; j += 0.1) {
						var f11 = new MuskatConfig(this.nullFunction);
						f11.meshMode = "delaunay";
						f11.seedMode = "floyd_steinberg";
						f11.Tthreshold = t;
						f11.gamma	 = g;
						f11.refine	 = true;
						f11.Tangle	 = a;
						f11.Tjoin	 = j;
						f11.preBackgroundSubtraction = false;
						f11.praBackgroundSubtraction = true;
						this.add(f11);

						var f12 = new MuskatConfig(this.nullFunction);
						f12.meshMode = "delaunay";
						f12.seedMode = "floyd_steinberg";
						f12.Tthreshold = t;
						f12.gamma	 = g;
						f12.refine	 = true;
						f12.Tangle	 = a;
						f12.Tjoin	 = j;
						f12.preBackgroundSubtraction = true;
						f12.praBackgroundSubtraction = false;
						this.add(f12);
					}
				}	
			}
		}

		// Delaunay Mesh QTree
		var zzz = new MuskatConfig(this.nullFunction);
		zzz.meshMode = "delaunay";
		zzz.maxDepth = 10;
		zzz.Tleaf	 = 0.0;
		zzz.Tinternal = 0.0;
		zzz.refine	 = false;
		zzz.preBackgroundSubtraction = false;
		zzz.praBackgroundSubtraction = false;
		this.add(zzz);

		for(var d = 10; d <= 10; d++) {
			for(var l = 0.0; l <= 1.0; l += 0.1) {
				for(var i = 0.0; i <= l; i += 0.1) {
					var c7 = new MuskatConfig(this.nullFunction);
					c7.meshMode = "delaunay";
					c7.maxDepth = d;
					c7.Tleaf	 = l;
					c7.Tinternal = i;
					c7.refine	 = false;
					c7.preBackgroundSubtraction = false;
					c7.praBackgroundSubtraction = false;
					this.add(c7);

					var c8 = new MuskatConfig(this.nullFunction);
					c8.meshMode = "delaunay";
					c8.maxDepth = d;
					c8.Tleaf	 = l;
					c8.Tinternal = i;
					c8.refine	 = false;
					c8.preBackgroundSubtraction = false;
					c8.praBackgroundSubtraction = true;
					this.add(c8);
								
					var c9 = new MuskatConfig(this.nullFunction);
					c9.meshMode = "delaunay";
					c9.maxDepth = d;
					c9.Tleaf	 = l;
					c9.Tinternal = i;
					c9.refine	 = false;
					c9.preBackgroundSubtraction = true;
					c9.praBackgroundSubtraction = false;
					this.add(c9);

					var c10 = new MuskatConfig(this.nullFunction);
					c10.meshMode = "delaunay";
					c10.maxDepth = d;
					c10.Tleaf	 = l;
					c10.Tinternal= i;
					c10.refine	 = false;
					c10.preBackgroundSubtraction = true;
					c10.praBackgroundSubtraction = true;
					this.add(c10);

					for(var a = 0.0; a <= 160.0; a += 20.0) {
						for(var j = 2.0; j <= 2.0; j += 0.1) {
							var c11 = new MuskatConfig(this.nullFunction);
							c11.meshMode = "delaunay";
							c11.maxDepth = 10;
							c11.Tleaf	 = l;
							c11.Tinternal= i;
							c11.refine	 = true;
							c11.Tangle	 = a;
							c11.Tjoin	 = j;
							c11.preBackgroundSubtraction = false;
							c11.praBackgroundSubtraction = true;
							this.add(c11);

							var c12 = new MuskatConfig(this.nullFunction);
							c12.meshMode = "delaunay";
							c12.maxDepth = 10;
							c12.Tleaf	 = l;
							c12.Tinternal= i;
							c12.refine	 = true;
							c12.Tangle	 = a;
							c12.Tjoin	 = j;
							c12.preBackgroundSubtraction = true;
							c12.praBackgroundSubtraction = false;
							this.add(c12);
						}
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

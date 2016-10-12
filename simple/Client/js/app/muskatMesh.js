class MuskatMesh {
	constructor(gl) {
		this._gl		= gl;
		this._vertices	= gl.createBuffer();
		this._indices	= gl.createBuffer();
		this._texCoords	= gl.createBuffer();
	}

	createPlane() {
		var array = new Float32Array([-1.0,  1.0,  1.0,
			 		  				   1.0,  1.0,  1.0,
					 				  -1.0, -1.0,  1.0,
			 		  				   1.0, -1.0,  1.0]);
		this.vertices = array;
		
		array = new Uint32Array([0, 1, 2,	1, 3, 2]);
		this.indices = array;

		array = new Float32Array([0.0, 0.0,
				 				  1.0, 0.0,
				 				  0.0, 1.0,
				 				  1.0, 1.0]);
		this.texCoords = array;
	}

	createComplexPlane(w, h, ow = 0, oh = 0) {
		var ws 	= w-1;
		var hs 	= h-1;
		var i 	= 0;
		var j 	= 0;
		var u	= 0;
		var v	= 0;
		var index = 0;

		var vb = new Float32Array(w * h * 3);
		var tb = new Float32Array(w * h * 2);
		var ib = new Uint32Array((w-1) * (h-1) * 6);

		var i_index = 0;
		var v_index = 0; 
		var t_index = 0;
		for(var y = oh; y < h; y++) {
			v = y / hs;
			j = 1 - v * 2;

			for(var x = ow; x < w; x++) {
				u = x / w;
				i = u * 2 - 1;

				vb[v_index] = i;	v_index++;
				vb[v_index] = j;	v_index++;	
				vb[v_index] = 1.0;	v_index++;

				tb[t_index] = u;	t_index++;
				tb[t_index] = v;	t_index++;

				index = y * w + x;
				ib[i_index] = index;	i_index++;
				ib[i_index] = index+1;	i_index++;
				ib[i_index] = index+w;	i_index++;
				ib[i_index] = index+1;	i_index++;
				ib[i_index] = index+w+1;i_index++;
				ib[i_index] = index+w;	i_index++;
			}
		}

		this.vertices 	= vb;
		this.texCoords 	= tb;
		this.indices 	= ib;
	}

	createCookieCutter(w, h, ow = 0, oh = 0) {
		var ws 	= w-1;
		var hs 	= h-1;
		var i 	= 0;
		var j 	= 0;
		var u	= 0;
		var v	= 0;
		var index = 0;

		var vb = new Float32Array(w * h * 3);
		var tb = new Float32Array(w * h * 2);
		var ib = new Uint32Array((w-1) * (h-1) * 6);

		var direction = true;
		var i_index = 0;
		var v_index = 0; 
		var t_index = 0;
		for(var y = oh; y < h; y++) {
			v = y / hs;
			j = 1 - v * 2;

			for(var x = ow; x < w; x++) {
				u = x / w;
				i = u * 2 - 1;

				vb[v_index] = i;	v_index++;
				vb[v_index] = j;	v_index++;	
				vb[v_index] = 1.0;	v_index++;

				tb[t_index] = u;	t_index++;
				tb[t_index] = v;	t_index++;

				index = y * w + x;

				if(direction) { // TODO
					ib[i_index] = index;	i_index++;
					ib[i_index] = index+1;	i_index++;
					ib[i_index] = index+w+1;i_index++;
					ib[i_index] = index;	i_index++;
					ib[i_index] = index+w+1;i_index++;
					ib[i_index] = index+w;	i_index++;
				} else {
					ib[i_index] = index;	i_index++;
					ib[i_index] = index+1;	i_index++;
					ib[i_index] = index+w;	i_index++;
					ib[i_index] = index+1;	i_index++;
					ib[i_index] = index+w+1;i_index++;
					ib[i_index] = index+w;	i_index++;
				}

				direction = !direction;
			}
		}

		this.vertices 	= vb;
		this.texCoords 	= tb;
		this.indices 	= ib;
	}

	createIsometricPlane(w, h, ow = 0, oh = 0) {
		var ws 	= w-1;
		var hs 	= h-1;
		var i 	= 0;
		var j 	= 0;
		var u	= 0;
		var v	= 0;
		var index = 0;

		var vb = new Float32Array(w * h * 3);
		var tb = new Float32Array(w * h * 2);
		var ib = new Uint32Array(3*(h-1) * (2*(w-2) + 1));
		//var ib = new Uint32Array((w-1) * (h-1) * 6);

		var direction = true;
		var i_index = 0;
		var v_index = 0; 
		var t_index = 0;
		for(var y = oh; y < h; y++) {
			v = y / hs;
			j = 1 - v * 2;

			for(var x = ow; x < w; x++) {
				u = x / w;
				i = u * 2 - 1;

				vb[v_index] = i;	v_index++;
				vb[v_index] = j;	v_index++;	
				vb[v_index] = 1.0;	v_index++;

				tb[t_index] = u;	t_index++;
				tb[t_index] = v;	t_index++;

				index = y * w + x;

				if(direction) {					
					ib[i_index] = index;	i_index++;
					ib[i_index] = index+w+1;i_index++;
					ib[i_index] = index+w;	i_index++;
					
					if(x  < ws) {
						ib[i_index] = index;	i_index++;
						ib[i_index] = index+1;	i_index++;
						ib[i_index] = index+w+1;i_index++;
					}
				} else {
					ib[i_index] = index;	i_index++;
					ib[i_index] = index+1;	i_index++;
					ib[i_index] = index+w;	i_index++;

					if(x  < ws) {
						ib[i_index] = index+1;	i_index++;
						ib[i_index] = index+w+1;i_index++;
						ib[i_index] = index+w;	i_index++;
					}
				}
			}
			
			direction = !direction;
		}

		this.vertices 	= vb;
		this.texCoords 	= tb;
		this.indices 	= ib;
	}


	get vertices() {
		return this._vertices;
	}

	set vertices(array) {
		this._gl.bindBuffer(this._gl.ARRAY_BUFFER, this._vertices);
		this._gl.bufferData(this._gl.ARRAY_BUFFER, array, this._gl.STATIC_DRAW);
		this._vertices.itemSize = 3;
		this._vertices.numItems = array.length;
	}

	get indices() {
		return this._indices;
	}

	set indices(array) {
		this._gl.bindBuffer(this._gl.ELEMENT_ARRAY_BUFFER, this._indices);
		this._gl.bufferData(this._gl.ELEMENT_ARRAY_BUFFER, array, this._gl.STATIC_DRAW);
		this._indices.itemSize = 1;
		this._indices.numItems = array.length;
	}

	get texCoords() {
		return this._texCoords;
	}

	set texCoords(array) {
		this._gl.bindBuffer(this._gl.ARRAY_BUFFER, this._texCoords);
		this._gl.bufferData(this._gl.ARRAY_BUFFER, array, this._gl.STATIC_DRAW);
		this._texCoords.itemSize = 2;
		this._texCoords.numItems = array.length;
	}
}

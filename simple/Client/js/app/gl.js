class MuskatGl {
	constructor(canvas) {
		try {
			this.canvas = canvas;
			this._gl 	= canvas.getContext("experimental-webgl");
			this._gl.viewportWidth	= canvas.width;
			this._gl.viewportHeight = canvas.height;
		} catch (e) {
			
		}

		if( !this._gl ) 
			alert("Could not initialise WebGl!");
	}

	get gl() {
		return this._gl;
	}

	compileShader(id) {
		var code = document.getElementById(id);

		if(!code) return null;

		var str = "";
		var k 	= code.firstChild;
		while(k) {
			if(k.nodeType == 3)
				str += k.textContent;

			k = k.nextSibling;
		}

		var shader;
		if(code.type == "x-shader/x-fragment")		shader = this._gl.createShader(this._gl.FRAGMENT_SHADER);
		else if(code.type == "x-shader/x-vertex")	shader = this._gl.createShader(this._gl.VERTEX_SHADER);
		else return null;

		this._gl.shaderSource(shader, str);
		this._gl.compileShader(shader);

		if(!this._gl.getShaderParameter(shader, this._gl.COMPILE_STATUS)) {
			alert(this._gl.getShaderInfoLog(shader));
			return null;
		}

		return shader;
	}

	linkShaderProgram(vertexShader, fragmentShader) {
		var shaderProgram = this._gl.createProgram();
		this._gl.attachShader(shaderProgram, vertexShader);
		this._gl.attachShader(shaderProgram, fragmentShader);
		this._gl.linkProgram(shaderProgram);

		if(!this._gl.getProgramParameter(shaderProgram, this._gl.LINK_STATUS)) {
			alert("Could not link shaders!");
		}

		return shaderProgram;
	}

	useShaderProgram(shaderProgram) {
		this._gl.useProgram(shaderProgram);
	}

	getVertexAttribLocation(shaderProgram, name) {
		var add;
		
		add = this._gl.getAttribLocation(shaderProgram, name);
		this._gl.enableVertexAttribArray(add);

		return add;
	} 

	getUniformLocation(shaderProgram, name) {
		return this._gl.getUniformLocation(shaderProgram, name);
	}
	
	handleLoadedTexture(texture, callback) {
		this._gl.bindTexture(this._gl.TEXTURE_2D, texture);
		this._gl.pixelStorei(this._gl.UNPACK_ALIGNMENT, true);
		this._gl.texImage2D(this._gl.TEXTURE_2D, 0, this._gl.RGBA, this._gl.RGBA, this._gl.UNSIGNED_BYTE, texture.image);
		this._gl.texParameteri(this._gl.TEXTURE_2D, this._gl.TEXTURE_MAG_FILTER, this._gl.NEAREST);
		this._gl.texParameteri(this._gl.TEXTURE_2D, this._gl.TEXTURE_MIN_FILTER, this._gl.NEAREST);
		this._gl.bindTexture(this._gl.TEXTURE_2D, null);

		if(callback !== 'undefine') callback();
	}

	createTextureFromUrl(url, callback) {
		var texture = this._gl.createTexture();
		
		texture.image = new Image();
		texture.image.onload = function() {
			this.handleLoadedTexture(texture, callback);
		}.bind(this);
		texture.image.src = url;

		return texture;
	}

	createTextureFromBase64(type, base64, callback) {
		var texture = this._gl.createTexture();
		
		texture.image = new Image();
		texture.image.onload = function() {
			this.handleLoadedTexture(texture, callback);
		}.bind(this);
		texture.image.src = 'data:image/'+ type +';base64, '+ base64.toString();

		return texture;
	}

	setTextureFromBase64(texture, type, base64, callback) {
		texture.image.onload = function() {
			this.handleLoadedTexture(texture, callback);
		}.bind(this);
		texture.image.src = 'data:image/'+ type +';base64, '+ base64.toString();
	}

	setUniformMatrix(uniformLocation, matrix) {
		this._gl.uniformMatrix4fv(uniformLocation, false, matrix);
	}

	degToRad(deg) {
		return deg * 0.0174532925199432957692369076848;
	}
}

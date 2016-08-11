class muskatGl {
	constructor(canvas) {
		try {
			this.gl = canvas.getContext("experimental-webgl");
			this.gl.viewportWidth	= canvas.width;
			this.gl.viewportHeight 	= canvas.height;
		} catch (e) {
			
		}

		if( !this.gl ) 
			alert("Could not initialise WebGl!");
	}

	get gl() {
		return this.gl;
	}

	getShaderById(id) {
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
		if(code.type == "x-shader/x-fragment")		shader = this.gl.createShader(this.gl.FRAGMENT_SHADER);
		else if(code.type == "x-shader/x-vertex")	shader = this.gl.createShader(this.gl.VERTEX_SHADER);
		else return null;

		this.gl.shaderSource(shader, str);
		this.gl.compileShader(shader);

		if(!this.gl.getShaderParameter(shader, this.gl.COMPILE_STATUS)) {
			alert(this.gl.getShaderInfoLog(shader));
			return null;
		}

		return shader;
	}

	linkShaders(shaderProgram) {
		var fragmentShader  = this.getShaderById("shader-fs");
		var vertexShader	= this.getShaderById("shader-vs");

		shaderProgram		= this.gl.createProgram();
		this.gl.attachShader(shaderProgram, vertexShader);
		this.gl.attachShader(shaderProgram, fragmentShader);
		this.gl.linkProgram(shaderProgram);

		if(!this.gl.getProgramParameter(shaderProgram, this.gl.LINK_STATUS)) {
			alert("Could not link shaders!");
		}
	}

	getVertexAttribLocation(shaderProgram, name) {
		var add;
		
		this.gl.getAttribLocation(shaderProgram, name);
		this.gl.enableVertexAttribArray(add);

		return add;
	} 

	getUniformLocation(shaderProgram, name) {
		return this.gl.getUniformLocation(shaderProgram, name);
	}

	handleLoadedTexture(texture) {
		gl.bindTexture(gl.TEXTURE_2D, texture);
		gl.pixelStorei(gl.UNPACK_ALIGNMENT, true);
		gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, texture.image);
		gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);
		gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);
		gl.bindTexture(gl.TEXTURE_2D, null);
	}

	createTextureFromBase64(type, base64, callback = null) {
		var texture = this.gl.createTexture();
		
		texture.image = new Image();
		texture.image.onload = function () {
			this.handleLoadedTexture(texture);

			if(callback != null) callback();
		}

		texture.image.src = 'data:image/'+ type +';base64, '+ base64.toString();

		return texture;
	}

	setTextureFromBase64(texture, type, base64, callback = null) {
		texture.image.onload = function () {
			this.handleLoadedTexture(texture);
			callback();

			if(callback != null) callback();
		}

		texture.image.src = 'data:image/'+ type +';base64, '+ base64.toString();
	}

	degToRad(deg) {
		return deg * 0.0174532925199432957692369076848;
	}
}

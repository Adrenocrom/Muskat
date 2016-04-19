/**************************************

Init.JS

GLOBALS:

	GlDevice


**************************************/

globalTextureStack = [];

function initGL(canvas) {
	try {
		GlDevice = canvas.getContext("experimental-webgl");
		GlDevice.viewportWidth = canvas.width;
		GlDevice.viewportHeight = canvas.height;
	} catch (e) {}
	if(!GlDevice) {
		alert("Could not initialise webgl!");
	}
}

function loadFileFromSrc(filename) {
	var text = "";
		
	jQuery.ajax({
		url: filename,
		dataType: "text",
		async: false
		}).done(function(data) {
			text = data;
	});

	return text;
}

function getShader(filename, type) {
	var shader;

	if(type == "fragment")
		shader = GlDevice.createShader(GlDevice.FRAGMENT_SHADER);	
	else if (type == "vertex")
		shader = GlDevice.createShader(GlDevice.VERTEX_SHADER);
	else return null;

	GlDevice.shaderSource(shader, loadFileFromSrc(filename));
	GlDevice.compileShader(shader);

	if (!GlDevice.getShaderParameter(shader, GlDevice.COMPILE_STATUS)) {
      alert(GlDevice.getShaderInfoLog(shader));
      return null;
   }

   return shader;
}

function degToRad(degrees) {
	return degrees * Math.PI / 180;
}

function handleLoadedTexture(texture) {
	GlDevice.bindTexture(GlDevice.TEXTURE_2D, texture);
	GlDevice.pixelStorei(GlDevice.UNPACK_FLIP_Y_WEBGL, true);
	GlDevice.texImage2D(GlDevice.TEXTURE_2D, 0, GlDevice.RGBA, GlDevice.RGBA, GlDevice.UNSIGNED_BYTE, texture.image);
	GlDevice.texParameteri(GlDevice.TEXTURE_2D, GlDevice.TEXTURE_MAG_FILTER, GlDevice.NEAREST);
	GlDevice.texParameteri(GlDevice.TEXTURE_2D, GlDevice.TEXTURE_MIN_FILTER, GlDevice.NEAREST);
	GlDevice.bindTexture(GlDevice.TEXTURE_2D, null);
}

function pushTexture(filename) {
	var texture = GlDevice.createTexture();
	texture.image = new Image();
	texture.image.onload = function() {
		handleLoadedTexture(texture);
		globalTextureStack.push(texture);
	}

	texture.image.src = filename;
}

function popTexture() {
	if(globalTextureStack > 0)
		globalTextureStack.pop();
}

function goInFullscreen() {
	var element = document.getElementById("graphic_device");
		 
	if (element.requestFullScreen) {
		element.requestFullScreen();
	} else if (element.mozRequestFullScreen) {
		element.mozRequestFullScreen();
	} else if (element.webkitRequestFullScreen) {
		element.webkitRequestFullScreen();
	}
}

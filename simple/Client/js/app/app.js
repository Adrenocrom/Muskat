/*
*	muskat client v0.0.1
*	Josef Schulz, 13.6.2016
*/

$(document).ready(function() {
	var debug_console = document.getElementById("textarea_debug");

	function debug(message) {
		debug_console.value += message + "\n";
		debug_console.scrollTop = debug_console.scrollHeight;
	}

	debug("muskat client v0.0.1")


	$('#button_toggle_debug').click(function toggleDebug() {
		debug_console.style.visibility = (debug_console.style.visibility == "visible" || debug_console.style.visibility == "") ? "hidden" : "visible";
	});

	var wsUri 		= "ws://localhost:1234";
	var websocket 	= null;
	var idCnt		= 0;

	$('#button_connect_to_server').click(function connectToServer() {
		wsUri = $('#text_server_uri').val();	

		document.getElementById("window_config").style.display = "none";
		document.getElementById("window_render").style.display = "block";

		debug("connecting to " + wsUri);

		startWs();
	});

	var gl;
	var shaderProgram;
	var meshTexture;
	var mvMatrix	 = mat4.create();
	var pMatrix 	 = mat4.create();
	var mvpMatrix 	 = mat4.create();
	var invMvpMatrix = mat4.create();
	
	var display;
	var canvas;
	var context;
	var theData;
	
	function initGL(canvas) {
		try {
    		gl = canvas.getContext("experimental-webgl");
        	gl.viewportWidth 	= canvas.width;
        	gl.viewportHeight 	= canvas.height;
    	} catch (e) {}
    	if (!gl) {
    		alert("Could not initialise WebGL, sorry :-(");
   		}
	}
    
	function getShader(gl, id) {
		var shaderScript = document.getElementById(id);
    	if (!shaderScript) {
    		return null;
		}

		var str = "";
		var k = shaderScript.firstChild;
		while (k) {
			if (k.nodeType == 3) {
				str += k.textContent;
			}
			k = k.nextSibling;
		}

		var shader;
		if (shaderScript.type == "x-shader/x-fragment") {
			shader = gl.createShader(gl.FRAGMENT_SHADER);
		} else if (shaderScript.type == "x-shader/x-vertex") {
			shader = gl.createShader(gl.VERTEX_SHADER);
		} else {
			return null;
		}

		gl.shaderSource(shader, str);
		gl.compileShader(shader);

		if (!gl.getShaderParameter(shader, gl.COMPILE_STATUS)) {
			alert(gl.getShaderInfoLog(shader));
			return null;
		}

		return shader;
	}

	function initShaders() {
		var fragmentShader = getShader(gl, "shader-fs");
	    var vertexShader = getShader(gl, "shader-vs");

		shaderProgram = gl.createProgram();
	    gl.attachShader(shaderProgram, vertexShader);
	    gl.attachShader(shaderProgram, fragmentShader);
	    gl.linkProgram(shaderProgram);
	
   		if (!gl.getProgramParameter(shaderProgram, gl.LINK_STATUS)) {
    		alert("Could not initialise shaders");
    	}

    	gl.useProgram(shaderProgram);

		shaderProgram.vertexPositionAttribute = gl.getAttribLocation(shaderProgram, "aVertexPosition");
    	gl.enableVertexAttribArray(shaderProgram.vertexPositionAttribute);

		shaderProgram.textureCoordAttribute = gl.getAttribLocation(shaderProgram, "aTextureCoord");
    	gl.enableVertexAttribArray(shaderProgram.textureCoordAttribute);
	
    	shaderProgram.invMvpMatrixUniform = gl.getUniformLocation(shaderProgram, "uINVMVPMatrix");
    	shaderProgram.mvpMatrixUniform = gl.getUniformLocation(shaderProgram, "uMVPMatrix");
		shaderProgram.samplerUniform = gl.getUniformLocation(shaderProgram, "uSampler");
	}

	function handleLoadedTexture(texture) {
		gl.bindTexture(gl.TEXTURE_2D, texture);
		//gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, true);
		gl.pixelStorei(gl.UNPACK_ALIGNMENT, true);
		gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, texture.image);
		gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);
		gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);
		gl.bindTexture(gl.TEXTURE_2D, null);
	}

	function initTexture() {
		meshTexture = gl.createTexture();
		meshTexture.image = new Image();
		meshTexture.image.onload = function () {
			handleLoadedTexture(meshTexture);
			drawScene();

		}

		meshTexture.image.src = "img/UV_Grid_Sm.jpg";
	}

	function setTexture(base64) {
		meshTexture.image.onload = function() {
			handleLoadedTexture(meshTexture);
			drawScene();
		}

		meshTexture.image.src = 'data:image/jpeg;base64, ' + base64.toString();
	}

	function setMatrixUniforms() {
		gl.uniformMatrix4fv(shaderProgram.invMvpMatrixUniform, false, invMvpMatrix);
		gl.uniformMatrix4fv(shaderProgram.mvpMatrixUniform, false, mvpMatrix);
	}

	var meshVertexPositionBuffer;
	var meshVertexTextureCoordBuffer;
	var meshVertexIndexBuffer;

	function initBuffers() {
		meshVertexPositionBuffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, meshVertexPositionBuffer);
		var vertices = [
			-1.0,  1.0,  1.0,
			 1.0,  1.0,  1.0,
			-1.0, -1.0,  1.0,
			 1.0, -1.0,  1.0
		];
		gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(vertices), gl.STATIC_DRAW);
		meshVertexPositionBuffer.itemSize = 3;
		meshVertexPositionBuffer.numItems = 4;

		meshVertexTextureCoordBuffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, meshVertexTextureCoordBuffer);
		var textureCoords = [
			0.0, 0.0,
			1.0, 0.0,
			0.0, 1.0,
			1.0, 1.0
		];
		gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(textureCoords), gl.STATIC_DRAW);
		meshVertexTextureCoordBuffer.itemSize = 2;
		meshVertexTextureCoordBuffer.numItems = 4;

		meshVertexIndexBuffer = gl.createBuffer();
		gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, meshVertexIndexBuffer);
		var meshVertexIndices = [
			0, 1, 2,      1, 3, 2 
		];
		gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, new Uint16Array(meshVertexIndices), gl.STATIC_DRAW);
		meshVertexIndexBuffer.itemSize = 1;
		meshVertexIndexBuffer.numItems = 6;
	}

	function degToRad(deg) {
		return deg * 0.0174532925199432957692369076848;
	}

	function drawScene() {
		gl.viewport(0, 0, gl.viewportWidth, gl.viewportHeight);
		gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

		mat4.perspective(pMatrix, degToRad(45.0), gl.viewportWidth / gl.viewportHeight, 0.1, 100.0);

		mat4.lookAt(mvMatrix, [0.0, 0.0, 0.0], [0.0, 0.0, -5.0], [0.0, 1.0, 0.0]);

		mat4.translate(mvMatrix, mvMatrix, [0.0, 0.0, -5.0]);

		mat4.multiply(mvpMatrix, pMatrix, mvMatrix);
	
		gl.bindBuffer(gl.ARRAY_BUFFER, meshVertexPositionBuffer);
		gl.vertexAttribPointer(shaderProgram.vertexPositionAttribute, meshVertexPositionBuffer.itemSize, gl.FLOAT, false, 0, 0);

		gl.bindBuffer(gl.ARRAY_BUFFER, meshVertexTextureCoordBuffer);
		gl.vertexAttribPointer(shaderProgram.textureCoordAttribute, meshVertexTextureCoordBuffer.itemSize, gl.FLOAT, false, 0, 0);

		gl.activeTexture(gl.TEXTURE0);
		gl.bindTexture(gl.TEXTURE_2D, meshTexture);
		gl.uniform1i(shaderProgram.samplerUniform, 0);

		gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, meshVertexIndexBuffer);
		setMatrixUniforms();
		gl.drawElements(gl.TRIANGLES, meshVertexIndexBuffer.numItems, gl.UNSIGNED_SHORT, 0);
	}


	function startWs() {
		try {
			if (typeof MozWebSocket == 'function')
				WebSocket = MozWebSocket;
			if ( websocket && websocket.readyState == 1 )
				websocket.close();

			websocket = new WebSocket( wsUri );
			websocket.onopen = function (evt) {
				debug("connected to " + wsUri);

				getPlaylistMessage();
				loadSceneMessage();
				initWebGl();

				getFrameMessage();
			};

			websocket.onmessage = function (evt) {
				var obj = JSON.parse(evt.data);

				if (typeof obj.result.rgb !== 'undefined') {
					setTexture(obj.result.rgb);
				}

/*
				//debug(evt.data);
				if(obj.result.rgb != "") {

			
					texture = new THREE.Texture(img); 
					img.onload = function() {
						texture.needsUpdate = true;
						mesh.material.map = texture;
						mesh.material.needsUpdate = true;
					};
					img.src = 'data:image/jpeg;base64, ' + obj.result.rgb.toString();

					if(obj.result.depth != "") {

						depthImg.onload = function() {
							context.drawImage(depthImg, 0, 0 );
							theData = context.getImageData(0, 0, depthImg.width, depthImg.height);
							
							for(var i = 0; i < theData.data.length; i += 4) {
									//debug(theData.data[i]);
							}
						}

						depthImg.src = 'data:image/png;base64, ' + obj.result.depth.toString();
					}
				}*/
            };

			websocket.onclose = function (evt) {
				debug("disconnected");
				
				document.getElementById("window_config").style.display = "block";
				document.getElementById("window_render").style.display = "none";
				$('#text_server_uri').val(wsUri);
			};

			websocket.onerror = function (evt) {
				debug('error: ' + evt.data);
			};
		} catch (exception) {
			debug('error: ' + exception);
		}
	}

	function closeWs() {
		if (websocket)
			websocket.close();
	}

	function initWebGl() {
		debug("init opengl");

		display = document.getElementById( 'display' );
		initGL(display);
		initShaders();
		initBuffers();
		initTexture();
		
		gl.clearColor(0.0, 0.0, 0.0, 1.0);
        gl.enable(gl.DEPTH_TEST);
		
		debug("opengl is ready");
	}

	function getFrameMessage() {
		var msg = {
			"jsonrpc" : "2.0",
			"method" : "getFrame",
			"params" : {
				"id" : 0
			},
			"id" : idCnt
		};
		//debug(JSON.stringify(msg));
		websocket.send(JSON.stringify(msg));
		idCnt++;
	}

	function getPlaylistMessage() {
		var msg = {
			"jsonrpc" : "2.0",
			"method"  : "getPlaylist",
			"params"  : {},
			"id"	  : idCnt
		};
		websocket.send(JSON.stringify(msg));
		idCnt++;
	}

	function loadSceneMessage() {
		var msg = {
			"jsonrpc" : "2.0",
			"method"  : "loadScene",
			"params"  : {
				"id" : 0
			},
			"id"	  : idCnt
		};
		websocket.send(JSON.stringify(msg));
		idCnt++;
	}

});


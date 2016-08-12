/*
*	muskat client v0.0.1
*	Josef Schulz, 13.6.2016
*/

$(document).ready(function() {
	var debug_console 	= document.getElementById("textarea_debug");
	var menu_widget 	= document.getElementById("window_setting");

	function debug(message) {
		debug_console.value += message + "\n";
		debug_console.scrollTop = debug_console.scrollHeight;
	}
	debug("muskat client v0.0.1")
	debug_console.style.visibility = "hidden";

	$('#button_toggle_debug').click(function toggleDebug() {
		debug("console: " + debug_console.style.visibility);
		debug_console.style.visibility = debug_console.style.visibility == "visible" ? "hidden" : "visible";
	});

	$('#button_toggle_menu').click(function toggleMenu(){
		menu_widget.style.visibility = menu_widget.style.visibility == "visible" ? "hidden" : "visible";
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

	var muGl;
	var shaderProgram;

	var depthTexture;
	var colorTexture;

	var mesh;
	
	function initWebGl() {
		debug("init opengl");
		muGl = new MuskatGl(document.getElementById('display'));
		
		debug("check extensions");
		debug("extension OES_element_index_uint");
		if(!muGl.gl.getExtension("OES_element_index_uint")) {
		   alert("This app needs 32bit indices and your device or browser doesn't appear to support them");
		   return;
		}

		debug("opengl is ready");
		
		debug("create shader program");
		var vs = muGl.compileShader("shader-vs");
		var fs = muGl.compileShader("shader-fs");
		shaderProgram = muGl.linkShaderProgram(vs, fs);

		muGl.useShaderProgram(shaderProgram);

		shaderProgram.vertexPositionAttrib 	= muGl.getVertexAttribLocation(shaderProgram, "aVertexPosition");
		shaderProgram.textureCoordAttrib	= muGl.getVertexAttribLocation(shaderProgram, "aTextureCoord");
		
		shaderProgram.invMvpMatrixUniform 	= muGl.getUniformLocation(shaderProgram, "uINVMVPMatrix");
    	shaderProgram.mvpMatrixUniform 		= muGl.getUniformLocation(shaderProgram, "uMVPMatrix");
		shaderProgram.colorSamplerUniform 	= muGl.getUniformLocation(shaderProgram, "uColorSampler");
		shaderProgram.depthSamplerUniform 	= muGl.getUniformLocation(shaderProgram, "uDepthSampler");

		debug("set default colors");
		muGl.gl.clearColor(0.0, 0.0, 0.2, 1.0);
        muGl.gl.enable(muGl.gl.DEPTH_TEST);

		debug("set default mesh");
		mesh = new MuskatMesh(muGl.gl);
		mesh.createPlane();
		resize(512, 512);

		debug("set default textures");
		colorTexture = muGl.createTextureFromUrl("img/UV_Grid_Sm.jpg", draw);
		depthTexture = muGl.createTextureFromUrl("img/default.png", draw);
	}

	function resize(w, h) {
		mesh.createComplexPlane(w, h);
	}

	var mvMatrix	 = mat4.create();
	var pMatrix 	 = mat4.create();
	var mvpMatrix 	 = mat4.create();
	var invMvpMatrix = mat4.create();

	function setInvMatrix(base64) {
		var m = base64.invMVP;

		console.log(base64.invMVP);

		mat4.set(invMvpMatrix, m[0], 	m[1], 	m[2], 	m[3],
							   m[4], 	m[5], 	m[6], 	m[7],
							   m[8], 	m[9], 	m[10],	m[11],
							   m[12], 	m[13], 	m[14], 	m[15]);
	}
	
	function draw() {
		var gl = muGl.gl;

		gl.viewport(0, 0, gl.viewportWidth, gl.viewportHeight);
		gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

		mat4.perspective(pMatrix, muGl.degToRad(90.0), gl.viewportWidth / gl.viewportHeight, 0.1, 100.0);

		mat4.lookAt(mvMatrix, [0.0, 0.0, 2.0], [0.0, 0.0, 0.0], [0.0, 1.0, 0.0]);

		mat4.translate(mvMatrix, mvMatrix, [0.0, 0.0, 0.0]);
		//mat3.rotate(mvMatrix, mvMatrix, degToRad(91));

		mat4.multiply(mvpMatrix, pMatrix, mvMatrix);
		gl.bindBuffer(gl.ARRAY_BUFFER, mesh.vertices);
		gl.vertexAttribPointer(shaderProgram.vertexPositionAttrib, mesh.vertices.itemSize, gl.FLOAT, false, 0, 0);

		gl.bindBuffer(gl.ARRAY_BUFFER, mesh.texCoords);
		gl.vertexAttribPointer(shaderProgram.textureCoordAttrib, mesh.texCoords.itemSize, gl.FLOAT, false, 0, 0);

		gl.activeTexture(gl.TEXTURE0);
		gl.bindTexture(gl.TEXTURE_2D, depthTexture);
		gl.uniform1i(shaderProgram.colorSamplerUniform, 0);


		gl.activeTexture(gl.TEXTURE1);
		gl.bindTexture(gl.TEXTURE_2D, colorTexture);
		gl.uniform1i(shaderProgram.colorSamplerUniform, 1);

		
		gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, mesh.indices);
		muGl.setUniformMatrix(shaderProgram.mvpMatrixUniform, mvpMatrix);
		muGl.setUniformMatrix(shaderProgram.invMvpMatrixUniform, invMvpMatrix);
		gl.drawElements(gl.TRIANGLES, mesh.indices.numItems, gl.UNSIGNED_INT, 0);
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
	
				if(typeof obj.result.scenes !== 'undefined') {
					playlist = obj.result;

					debug("numScenes: " + playlist.scenes.length);
				}

				
				if(typeof obj.result.rgb !== 'undefined')
					muGl.setTextureFromBase64(colorTexture, "jpeg", obj.result.rgb, draw);

				if(typeof obj.result.depth !== 'undefined')
					muGl.setTextureFromBase64(depthTexture, "png", obj.result.depth, draw);
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

	
	function getFrameMessage() {
		var msg = {
			"jsonrpc" : "2.0",
			"method" : "getFrame",
			"params" : {
				"id" : 0 	/*	: 	info.id,
				"pos"		: 	info.pos,
				"lookat"	: 	info.lookat,
				"up"		:	info.up,
				"near"		:	info.near,
				"far"		:	info.far,
				"offangle"	: 	info.offangle
*/
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


/*
*	muskat client v0.0.1
*	Josef Schulz, 13.6.2016
*/

$(document).ready(function() {
	var debug_console 	= document.getElementById("textarea_debug");
	var menu_widget 	= document.getElementById("window_setting");
	var render_widget	= document.getElementById("window_render");
	var g_width			= 512;
	var g_height		= 512;
	var g_isOut			= false;

	function debug(message) {
		debug_console.value += message + "\n";
		debug_console.scrollTop = debug_console.scrollHeight;
	}
	debug("muskat client v0.0.1")
	//debug_console.style.visibility = "hidden";
	$('#window_setting').animate({width: "0px"}, 0);
	$('#textarea_debug').slideUp(0);

	$('#button_toggle_debug').click(function toggleDebug() {
		$('#textarea_debug').slideToggle(1000);
	});

	$('#button_toggle_menu').click(function toggleMenu(){
		if(!g_isOut) {
			$('#window_render').animate({
				margin: "50px auto 0px " +((window.innerWidth/2) - g_width) / 2
			}, 500);

			$('#window_setting').animate({
				width: "50%"
			}, 500);
		} else {
			$('#window_render').animate({
				margin: "50px auto 0px "+ (window.innerWidth - g_width) / 2
			}, 500);

			$('#window_setting').animate({
				width: "0px"
			}, 500);
		}

		g_isOut = !g_isOut;
	});

	$( "#select_scenes" ).change(function() {
		setScene(g_playlist.scenes[this.selectedIndex]);
		loadSceneMessage(this.selectedIndex);
		g_scene_index = this.selectedIndex;
		getFrameMessage(0);
	});

	$( '#button_run_scene' ).click(function() {
		newMessureMessage();
	});

	$( '#button_render_single_frame' ).click(function() {
		downloadFrameToPNG();
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

	$('#slider_jpeg_quality').on('input', function () {
	    $('#text_jpeg_quality').val($('#slider_jpeg_quality').val());
		g_config.textureCompressionQuality = parseInt($('#slider_jpeg_quality').val());
	});

	$('#slider_png_quality').on('input', function () {
	    $('#text_png_quality').val($('#slider_png_quality').val());
		g_config.textureCompressionQuality = parseInt($('#slider_png_quality').val());
	});

	$('#slider_mesh_quality').on('input', function () {
	    $('#text_mesh_quality').val($('#slider_mesh_quality').val());
		g_config.meshCompressionQualtiy = parseInt($('#slider_mesh_quality').val());
	});

	$('input[name=texture]').on('change', function() {
		g_config.textureCompressionMethod = $('input[name=texture]:checked').val(); 
	});

	$('input[name=mesh]').on('change', function() {
		g_config.meshCompressionMethod = $('input[name=mesh]:checked').val();

		if(g_config.meshCompressionMethod == "8bit") {
			g_mesh_compression_method = 0;
		}
		else if(g_config.meshCompressionMethod == "16bit") {
			g_mesh_compression_method = 1;
		}
		else {
			g_mesh_compression_method = 2;
		}
	});


	var muGl;
	var shaderProgram;

	var depthTexture;
	var colorTexture;

	var g_config;
	var g_mesh;
	var g_playlist;
	var g_scene_index;
	var g_mesh_compression_method;

	initConfig();
	
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
		shaderProgram.depthUniform			= muGl.getUniformLocation(shaderProgram, "uDepth");

		debug("set default colors");
		muGl.gl.clearColor(0.0, 0.0, 0.0, 1.0);
        muGl.gl.enable(muGl.gl.DEPTH_TEST);

		debug("set default mesh");
		g_mesh = new MuskatMesh(muGl.gl);
		g_mesh.createPlane();
		resize(512, 512);
		g_mesh_compression_method = 0;

		debug("set default textures");
		colorTexture = muGl.createTextureFromUrl("img/UV_Grid_Sm.jpg", draw);
		depthTexture = muGl.createTextureFromUrl("img/default.png", draw);
	}

	function initConfig() {
		g_config = new MuskatConfig(updateConfig);
	}

	function updateConfig() {
		setConfigMessage();
		getFrameMessage(0);
	}

	function resize(w, h) {
		//g_mesh.createCookieCutter(w, h);
		g_mesh.createIsometricPlane(w, h);
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

		mat4.identity(invMvpMatrix);

		mat4.perspective(pMatrix, muGl.degToRad(90.0), gl.viewportWidth / gl.viewportHeight, 0.1, 100.0);
		mat4.lookAt(mvMatrix, [0.0, 0.0, 2.0], [0.0, 0.0, 0.0], [0.0, 1.0, 0.0]);
		mat4.translate(mvMatrix, mvMatrix, [0.0, 0.0, 0.0]);
		mat4.multiply(mvpMatrix, pMatrix, mvMatrix);
		
		gl.bindBuffer(gl.ARRAY_BUFFER, g_mesh.vertices);
		gl.vertexAttribPointer(shaderProgram.vertexPositionAttrib, g_mesh.vertices.itemSize, gl.FLOAT, false, 0, 0);

		gl.bindBuffer(gl.ARRAY_BUFFER, g_mesh.texCoords);
		gl.vertexAttribPointer(shaderProgram.textureCoordAttrib, g_mesh.texCoords.itemSize, gl.FLOAT, false, 0, 0);

		gl.activeTexture(gl.TEXTURE0);
		gl.bindTexture(gl.TEXTURE_2D, depthTexture);
		gl.uniform1i(shaderProgram.colorSamplerUniform, 0);

		gl.activeTexture(gl.TEXTURE1);
		gl.bindTexture(gl.TEXTURE_2D, colorTexture);
		gl.uniform1i(shaderProgram.colorSamplerUniform, 1);
		
		gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, g_mesh.indices);
		muGl.setUniformMatrix(shaderProgram.mvpMatrixUniform, mvpMatrix);
		muGl.setUniformMatrix(shaderProgram.invMvpMatrixUniform, invMvpMatrix);
		gl.uniform1i(shaderProgram.depthUniform, g_mesh_compression_method);
		gl.drawElements(gl.TRIANGLES, g_mesh.indices.numItems, gl.UNSIGNED_INT, 0);
	}

	function drawFirstFrame() {
		var gl = muGl.gl;
		var scene = g_playlist.scenes[g_scene_index];
		var frame = scene.frames[0];
		
		mat4.perspective(pMatrix, muGl.degToRad(scene.aperture), gl.viewportWidth / gl.viewportHeight, frame.near, frame.far);
		mat4.lookAt(mvMatrix, frame.pos, frame.lookat, frame.up);
		mat4.translate(mvMatrix, mvMatrix, frame.lookat);
		mat4.multiply(invMvpMatrix, pMatrix, mvMatrix);
		mat4.invert(invMvpMatrix, invMvpMatrix);


		mat4.perspective(pMatrix, muGl.degToRad(scene.aperture), gl.viewportWidth / gl.viewportHeight, frame.near, frame.far);
		mat4.lookAt(mvMatrix, frame.pos, frame.lookat, frame.up);
		mat4.translate(mvMatrix, mvMatrix, frame.lookat);
		mat4.multiply(mvpMatrix, pMatrix, mvMatrix);
			
		drawEx();
	}

	function drawEx() {
		var gl = muGl.gl;

		gl.viewport(0, 0, gl.viewportWidth, gl.viewportHeight);
		gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

		gl.bindBuffer(gl.ARRAY_BUFFER, g_mesh.vertices);
		gl.vertexAttribPointer(shaderProgram.vertexPositionAttrib, g_mesh.vertices.itemSize, gl.FLOAT, false, 0, 0);

		gl.bindBuffer(gl.ARRAY_BUFFER, g_mesh.texCoords);
		gl.vertexAttribPointer(shaderProgram.textureCoordAttrib, g_mesh.texCoords.itemSize, gl.FLOAT, false, 0, 0);

		gl.activeTexture(gl.TEXTURE0);
		gl.bindTexture(gl.TEXTURE_2D, depthTexture);
		gl.uniform1i(shaderProgram.colorSamplerUniform, 0);

		gl.activeTexture(gl.TEXTURE1);
		gl.bindTexture(gl.TEXTURE_2D, colorTexture);
		gl.uniform1i(shaderProgram.colorSamplerUniform, 1);
		
		gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, g_mesh.indices);
		muGl.setUniformMatrix(shaderProgram.mvpMatrixUniform, mvpMatrix);
		muGl.setUniformMatrix(shaderProgram.invMvpMatrixUniform, invMvpMatrix);
		gl.drawElements(gl.TRIANGLES, g_mesh.indices.numItems, gl.UNSIGNED_INT, 0);
	}

	function setPlaylist(playlist) {
		g_playlist = playlist;

		var select = document.getElementById("select_scenes");

		for(var i = 0; i < playlist.scenes.length; i++) {
			var option 	= document.createElement("option");
			option.text = playlist.scenes[i].name;
			select.add(option);
		}

		select.selectedIndex = 0;
		setScene(playlist.scenes[0]);
		loadSceneMessage(0);
		g_scene_index = 0;
	}

	function setScene(scene) {
		var select_frames_min = document.getElementById("select_frames_min");
		var select_frames_max = document.getElementById("select_frames_max");
		$("#select_frames_min").empty();
		$("#select_frames_max").empty();
		
		for(var i = 0; i < scene.frames.length; i++) {
			var option_min = document.createElement("option");
			option_min.text = i;
			select_frames_min.add(option_min);

			var option_max = document.createElement("option");
			option_max.text = i;
			select_frames_max.add(option_max);
		}

		select_frames_max.selectedIndex = scene.frames.length-1;
	}

	function saveFrameToPNG(id) {
		var data = muGl.canvas.toDataURL("image/png;base64;");
		saveFrameMessage(id, data);
	}

	function downloadFrameToPNG() {
		drawFirstFrame();

		var data = muGl.canvas.toDataURL("image/png;base64;");
		alert(data.toString());
		var download = document.createElement('a');
		document.body.appendChild(download);
		download.setAttribute("href", data);
		//download.setAttribute("target", '_blank');
		download.setAttribute("download", 'frame.png');
		download.click();
		document.body.removeChild(download);
	}

	function runScene() {
		var gl = muGl.gl;
		var select_frames_min = document.getElementById("select_frames_min");
		var select_frames_max = document.getElementById("select_frames_max");

		var frame_min = select_frames_min.selectedIndex;
		var frame_max = select_frames_max.selectedIndex;

		var scene = g_playlist.scenes[g_scene_index];
		var frame = scene.frames[0];
		
		mat4.perspective(pMatrix, muGl.degToRad(scene.aperture), gl.viewportWidth / gl.viewportHeight, frame.near, frame.far);
		mat4.lookAt(mvMatrix, frame.pos, frame.lookat, frame.up);
		mat4.translate(mvMatrix, mvMatrix, frame.lookat);
		mat4.multiply(invMvpMatrix, pMatrix, mvMatrix);
		mat4.invert(invMvpMatrix, invMvpMatrix);

		for(var i = frame_min; i <= frame_max; i++) {
			frame = scene.frames[i];

			mat4.perspective(pMatrix, muGl.degToRad(scene.aperture), gl.viewportWidth / gl.viewportHeight, frame.near, frame.far);
			mat4.lookAt(mvMatrix, frame.pos, frame.lookat, frame.up);
			mat4.translate(mvMatrix, mvMatrix, frame.lookat);
			mat4.multiply(mvpMatrix, pMatrix, mvMatrix);
			
			drawEx();
			saveFrameToPNG(i);
		}

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

				setConfigMessage();
				getPlaylistMessage();
				initWebGl();

				getFrameMessage(0);
			};

			websocket.onmessage = function (evt) {
				var obj = JSON.parse(evt.data);
	
				if(typeof obj.result.scenes !== 'undefined') {
					playlist = obj.result;
					
					setPlaylist(playlist);
					debug(JSON.stringify(playlist));
					debug("numScenes: " + playlist.scenes.length);
				}

				
				if(typeof obj.result.rgb !== 'undefined') {
					muGl.setTextureFromBase64(colorTexture, g_config.textureCompressionMethod, obj.result.rgb, drawFirstFrame);

					muGl.setTextureFromBase64(depthTexture, "png", obj.result.depth, drawFirstFrame);
				}

				if(typeof obj.result.newMessureReady !== 'undefined') {
					runScene();
				}
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

	function sendMessage(method, params) {
		var msg = {
			"jsonrpc" 	: "2.0",
			"method"	: method,
			"params"	: params,
			"id"		: idCnt
		};

		websocket.send(JSON.stringify(msg));
		idCnt++;
	}
	
	function getFrameMessage(id) {
		var params = {"id" : id};
		sendMessage("getFrame", params);
	}

	function saveFrameMessage(id, base64) {
		var params = {
			"id" 	: id,
			"rgb" 	: base64
		};
		sendMessage("saveFrame", params);
	}

	function getPlaylistMessage() {
		sendMessage("getPlaylist", {});
	}

	function loadSceneMessage(id) {
		var params = {"id" : id};
		sendMessage("loadScene", params);
	}

	function newMessureMessage() {
		sendMessage("newMessure", {});
	}

	function setConfigMessage() {
		var params = {
			"textureCompressionMethod" 	: g_config.textureCompressionMethod,
			"textureCompressionQuality"	: g_config.textureCompressionQuality,
			"meshCompressionMethod"		: g_config.meshCompressionMethod,
			"meshCompressionQualtiy"	: g_config.meshCompressionQualtiy
		};

		debug(JSON.stringify(params));
		sendMessage("setConfig", params);
	}

});


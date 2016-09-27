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

	debug_console.style.height = (window.innerHeight - 60) + 'px';
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

	function closeMenu() {
		if(g_isOut) {
			$('#window_render').animate({
				margin: "50px auto 0px "+ (window.innerWidth - g_width) / 2
			}, 0);
			
			$('#window_setting').animate({
				width: "0px"
			}, 0);
		}
	}

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

	$( '#button_show_frame' ).click(function() {
		var select_frames_max = document.getElementById("select_frames_max");
		drawFrame(select_frames_max.selectedIndex);
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

	$('input[name=texture]').on('change', function() {
		g_config.textureCompressionMethod = $('input[name=texture]:checked').val(); 
	});

	$('#slider_jpeg_quality').on('input', function () {
	    $('#text_jpeg_quality').val($('#slider_jpeg_quality').val());
		g_config.textureCompressionQuality = parseInt($('#slider_jpeg_quality').val());
	});

	$('#slider_png_quality').on('input', function () {
	    $('#text_png_quality').val($('#slider_png_quality').val());
		g_config.textureCompressionQuality = parseInt($('#slider_png_quality').val());
	});

	$('input[name=mesh_mode]').on('change', function() {
		g_config.meshMode = $('input[name=mesh_mode]:checked').val(); 
	
		if(g_config.meshMode == "full") {
			resize(g_config.width, g_config.height);
		}
	});

	$('input[name=precision]').on('change', function() {
		g_config.meshPrecision = $('input[name=precision]:checked').val(); 
	});

	$('#slider_mesh_quality').on('input', function () {
	    $('#text_mesh_quality').val($('#slider_mesh_quality').val());
		g_config.meshCompression = parseInt($('#slider_mesh_quality').val());
	});

	$('input[name=grid_type]').on('change', function() {
		g_config.gridType = $('input[name=grid_type]:checked').val();

		resize(g_config.width, g_config.height);
	});

	$('#text_max_depth').on('change', function() {
		g_config.maxDepth = parseInt($('#text_max_depth').val());
	});

	$('#text_T_internal').on('change', function() {
		g_config.Tinternal = parseFloat($('#text_T_internal').val());
		
		if(g_config.Tinternal >= g_config.Tleaf) {
			$('#text_T_leaf').val(g_config.Tinternal);
		}
	});

	$('#text_T_leaf').on('change', function() {
		g_config.Tleaf = parseFloat($('#text_T_leaf').val());
	});
	
	var muGl;
	var shaderProgram;

	var depthTexture;
	var colorTexture;

	var g_config;
	var g_mesh;
	var g_playlist;
	var g_scene_index;
	var g_time_start;
	var g_time_end;

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
		shaderProgram.resolution			= muGl.getUniformLocation(shaderProgram, "uResolution");

		debug("set default colors");
		muGl.gl.clearColor(0.0, 0.0, 0.0, 1.0);
        muGl.gl.enable(muGl.gl.DEPTH_TEST);

		debug("set default mesh");
		g_mesh = new MuskatMesh(muGl.gl);
		g_mesh.createPlane();
		resize(512, 512);

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
		if(g_config.gridType == "default") {
			g_mesh.createComplexPlane(w, h);
		} else if(g_config.gridType == "cookie_cutter") {
			g_mesh.createCookieCutter(w, h);
		} else {
			g_mesh.createIsometricPlane(w, h);
		}
	}

	var mvMatrix	 = mat4.create();
	var pMatrix 	 = mat4.create();
	var mvpMatrix 	 = mat4.create();
	var invMvpMatrix = mat4.create();
	
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
		gl.uniform1i(shaderProgram.depthUniform, g_config.meshState);
		gl.drawElements(gl.TRIANGLES, g_mesh.indices.numItems, gl.UNSIGNED_INT, 0);

		//gl.flush();
	}

	function drawFirstFrame() {
		drawFrame(0);
	}

	function drawFrame(to) {
		var gl = muGl.gl;
		var scene = g_playlist.scenes[g_scene_index];
		var frame = scene.frames[0];
		var frameTo = scene.frames[to];
		
		mat4.perspective(pMatrix, muGl.degToRad(scene.aperture), gl.viewportWidth / gl.viewportHeight, frame.near, frame.far);
		mat4.lookAt(mvMatrix, frame.pos, frame.lookat, frame.up);
		mat4.translate(mvMatrix, mvMatrix, frame.lookat);
		mat4.multiply(invMvpMatrix, pMatrix, mvMatrix);
		mat4.invert(invMvpMatrix, invMvpMatrix);

		mat4.perspective(pMatrix, muGl.degToRad(scene.aperture), gl.viewportWidth / gl.viewportHeight, frameTo.near, frameTo.far);
		mat4.lookAt(mvMatrix, frameTo.pos, frameTo.lookat, frameTo.up);
		mat4.translate(mvMatrix, mvMatrix, frameTo.lookat);
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
		
		gl.uniform1i(shaderProgram.depthUniform, g_config.meshState);
		gl.uniform2f(shaderProgram.resolution, g_config.width, g_config.height);

		muGl.setUniformMatrix(shaderProgram.mvpMatrixUniform, mvpMatrix);
		muGl.setUniformMatrix(shaderProgram.invMvpMatrixUniform, invMvpMatrix);
		
		gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, g_mesh.indices);
		gl.drawElements(gl.TRIANGLES, g_mesh.indices.numItems, gl.UNSIGNED_INT, 0);

		// TODO
		gl.flush();

		// end messure
		g_time_end = new Date().getTime();
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

	function saveFrameToPNG(id, time) {
		var data = muGl.canvas.toDataURL("image/png;base64;");
		saveFrameMessage(id, data, time);
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
			// messure time
			g_time_start = new Date().getTime();

			frame = scene.frames[i];

			mat4.perspective(pMatrix, muGl.degToRad(scene.aperture), gl.viewportWidth / gl.viewportHeight, frame.near, frame.far);
			mat4.lookAt(mvMatrix, frame.pos, frame.lookat, frame.up);
			mat4.translate(mvMatrix, mvMatrix, frame.lookat);
			mat4.multiply(mvpMatrix, pMatrix, mvMatrix);
			
			drawEx();
			saveFrameToPNG(i, g_time_end-g_time_start);
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
					//debug(JSON.stringify(playlist));
					debug("numScenes: " + playlist.scenes.length);
				}


				if(typeof obj.result.rgb !== 'undefined') {
					muGl.setTextureFromBase64(colorTexture, g_config.textureCompressionMethod, obj.result.rgb, drawFirstFrame);
				}
				
				if(typeof obj.result.depth !== 'undefined') {
					muGl.setTextureFromBase64(depthTexture, "png", obj.result.depth, drawFirstFrame);
				}
				
				if(typeof obj.result.indices !== 'undefined') {
					debug(JSON.stringify(obj.result.indices));
					
					var indices  = JSON.parse(obj.result.indices);
					var vertices = JSON.parse(obj.result.vertices);

					g_mesh.indices = new Uint32Array(indices);
					g_mesh.vertices = new Float32Array(vertices);
					g_mesh.texCoords = new Float32Array(obj.result.numTexCoord);


					debug(obj.result.indices);
					debug(obj.result.vertices);
					debug(obj.result.numTexCoord);

					drawFirstFrame();
				}

				if(typeof obj.result.newMessureReady !== 'undefined') {
					runScene();
				}
            };

			websocket.onclose = function (evt) {
				debug("disconnected");

				closeMenu();
				
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
			"width"						: g_config.width,
			"height"					: g_config.height,
			"meshWidth"					: g_config.meshWidth,
			"meshHeight"				: g_config.meshHeight,
			"textureCompressionMethod" 	: g_config.textureCompressionMethod,
			"textureCompressionQuality"	: g_config.textureCompressionQuality,
			"meshMode"					: g_config.meshMode,
			"gridType"					: g_config.gridType,
			"meshPercesion"				: g_config.meshPercesion,
			"meshCompression"			: g_config.meshCompression,
			"maxDepth"					: g_config.maxDepth,
			"Tleaf"						: g_config.Tleaf,
			"Tinternal"					: g_config.Tinternal,
			"Tangle"					: g_config.Tangle,
			"Tjoin"						: g_config.Tjoin,
			"useBackground"				: g_config.useBackground
		};

		debug(JSON.stringify(params));
		sendMessage("setConfig", params);
	}

});


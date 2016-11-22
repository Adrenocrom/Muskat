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

	$( window ).resize(function() {
  		debug_console.style.height = (window.innerHeight - 60) + 'px';
	});
	
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

			g_isOut = false;
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

	$('#button_evaluation_set_default').click(function() {
		g_evaluator.setDefault();
		$('#label_evaluation').text("    0 / " + g_evaluator.length + " | scene " + g_scene_list[g_scene_list_index]);
	});

	$('#button_evaluation_set_config').click(function() {
		g_evaluator.setFromConfig(g_config);
		$('#label_evaluation').text("    0 / " + g_evaluator.length + " | scene " + g_scene_list[g_scene_list_index]);
	});

	$( '#button_run_scene' ).click(function() {
		g_scene_list_index = 0;

		setScene(g_playlist.scenes[g_scene_list[g_scene_list_index]]);
		loadSceneMessage(g_scene_list[g_scene_list_index]);
		g_scene_index = g_scene_list[g_scene_list_index];

		
		$('fieldset').prop('disabled', true);
		
		g_evaluation_id = 0;
		runEvaluation();
	});

	$( '#button_render_single_frame' ).click(function() {
		var select_frames_max = document.getElementById("select_frames_max");
		drawFrame(select_frames_max.selectedIndex);
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

	$('#select_image_size').change(function() {
		if(this.selectedIndex == 0) {
			g_config.meshWidth  = g_config.width;
			g_config.meshHeight = g_config.height;
		}
		else if(this.selectedIndex == 1) {
			g_config.meshWidth  = g_config.width / 2;
			g_config.meshHeight = g_config.height / 2;
		}
		else if(this.selectedIndex == 2) {
			g_config.meshWidth  = g_config.width / 4;
			g_config.meshHeight = g_config.height / 4;
		}

		resize(g_config.meshWidth, g_config.meshHeight);
	});

	$('#select_five_pass').change(function() {
		if(this.selectedIndex == 0) {
			g_config.fivePass = true;
		} else {
			g_config.fivePass = false;
		}
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
			resize(g_config.meshWidth, g_config.meshHeight);
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

		resize(g_config.meshWidth, g_config.meshHeight);
	});

	$('input[name=seed_mode]').on('change', function() {
		g_config.seedMode = $('input[name=seed_mode]:checked').val();
	});

	$('#text_max_depth').on('change', function() {
		g_config.maxDepth = parseInt($('#text_max_depth').val());
	});

	$('#text_T_internal').on('change', function() {
		g_config.Tinternal = parseFloat($('#text_T_internal').val());
	});

	$('#text_T_leaf').on('change', function() {
		g_config.Tleaf = parseFloat($('#text_T_leaf').val());
	});

	$('#text_T_angle').on('change', function() {
		g_config.Tangle = parseFloat($('#text_T_angle').val());
	});
	
	$('#text_T_join').on('change', function() {
		g_config.Tjoin = parseFloat($('#text_T_join').val());
	});

	$('#text_T_grad').on('change', function() {
		g_config.Tgrad = parseFloat($('#text_T_grad').val());
	});

	$('#text_T_threshold').on('change', function() {
		g_config.Tthreshold = parseFloat($('#text_T_threshold').val());
	});

	$('#text_gamma').on('change', function() {
		g_config.gamma = parseFloat($('#text_gamma').val());
	});

	$('#check_refine').change(function() {
		g_config.refine = $('#check_refine').prop('checked');
	});

	$('#check_pre_background').change(function() {
		g_config.preBackgroundSubtraction = $('#check_pre_background').prop('checked');
	});

	$('#check_pra_background').change(function() {
		g_config.praBackgroundSubtraction = $('#check_pra_background').prop('checked');
	});

	$('#check_smooth_depth').change(function() {
		g_config.smoothDepth = $('#check_smooth_depth').prop('checked');
	});
	
	$('#check_pointcloud').change(function() {
		g_config.pointcloud = !g_config.pointcloud;
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
	var g_evaluator;
	var g_evaluation_id		= 0;
	var g_scene_list 		= [6,7];
	var g_scene_list_index 	= 0;
	var g_callback_cnt 		= 0;

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
		
    	shaderProgram.mvpMatrixUniform 		= muGl.getUniformLocation(shaderProgram, "uMVPMatrix");
		shaderProgram.colorSamplerUniform 	= muGl.getUniformLocation(shaderProgram, "uColorSampler");
		shaderProgram.depthSamplerUniform 	= muGl.getUniformLocation(shaderProgram, "uDepthSampler");
		shaderProgram.depthUniform			= muGl.getUniformLocation(shaderProgram, "uDepth");
		shaderProgram.resolution			= muGl.getUniformLocation(shaderProgram, "uResolution");
		shaderProgram.Tgrad					= muGl.getUniformLocation(shaderProgram, "uTgrad");
		shaderProgram.alpha					= muGl.getUniformLocation(shaderProgram, "uAlpha");

		debug("set default colors");
		muGl.gl.clearColor(0.0, 0.0, 0.0, 1.0);
        muGl.gl.enable(muGl.gl.DEPTH_TEST);
		
		muGl.gl.disable(muGl.gl.CULL_FACE)
		//muGl.gl.enable(muGl.gl.CULL_FACE);
		
		muGl.gl.enable(muGl.gl.BLEND);
		muGl.gl.blendFunc(muGl.gl.SRC_ALPHA, muGl.gl.ONE_MINUS_SRC_ALPHA);
		//muGl.gl.blendFunc(muGl.gl.ONE, muGl.gl.ONE_MINUS_SRC_ALPHA);
		//muGl.gl.blendFunc(muGl.gl.SRC_ALPHA_SATURATE, muGl.gl.ONE);

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
		g_evaluator = new MuskatEvaluator();
		$('#label_evaluation').text("    0 / " + g_evaluator.length + " | scene " + g_scene_list[g_scene_list_index]);
	}

	function updateConfig() {
		if(g_config.meshMode == "full")
			resize(g_config.meshWidth, g_config.meshWidth);
		setConfigMessage();
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
	var resMvpMatrix = mat4.create();
	
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
		gl.uniform1i(shaderProgram.depthUniform, g_config.meshState);
		gl.drawElements(gl.TRIANGLES, g_mesh.indices.numItems, gl.UNSIGNED_INT, 0);

		gl.flush();
	}

	function callback_draw() {
		drawFrame(0);
		g_callback_cnt++;

		if(g_callback_cnt == -1) {
			newMessureMessage(g_scene_index, g_evaluation_id, g_config.name, g_config.shortName);
			g_callback_cnt = 0;
		}
		else if(g_callback_cnt == 2) {
			g_callback_cnt = 0;
		}
	}

	function drawFrame(to) {
		var gl = muGl.gl;
		var scene = g_playlist.scenes[g_scene_index];
		var frame	= scene.frames[0];
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
		mat4.multiply(resMvpMatrix, mvpMatrix, invMvpMatrix);
			
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
		gl.uniform2f(shaderProgram.resolution, g_config.meshWidth, g_config.meshHeight);
		gl.uniform1f(shaderProgram.Tgrad, g_config.Tgrad);

		gl.uniform1f(shaderProgram.alpha, 1.0);
		muGl.setUniformMatrix(shaderProgram.mvpMatrixUniform, resMvpMatrix);
		
		gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, g_mesh.indices);

		if(g_config.meshState == 2) {
			// draw delaunay 
			gl.disable(gl.CULL_FACE);
			gl.drawElements(gl.TRIANGLES, g_mesh.indices.numItems, gl.UNSIGNED_INT, 0);
		} else {
			if(g_config.pointcloud) {
				gl.enable(gl.VERTEX_PROGRAM_POINT_SIZE); 
				gl.disable(gl.CULL_FACE);
				gl.drawElements(gl.POINTS, g_mesh.indices.numItems, gl.UNSIGNED_INT, 0);
			} else {
				if(g_config.fivePass) {
					/*
 					*	Quint Pas
 					*/
   		
					gl.disable(gl.CULL_FACE);
					gl.uniform1f(shaderProgram.alpha, 0.0);
					gl.depthFunc(gl.LESS);
					gl.drawElements(gl.TRIANGLES, g_mesh.indices.numItems, gl.UNSIGNED_INT, 0);

					gl.enable(gl.CULL_FACE);
					gl.cullFace(gl.FRONT);
		
					gl.depthFunc(gl.ALWAYS);
					gl.drawElements(gl.TRIANGLES, g_mesh.indices.numItems, gl.UNSIGNED_INT, 0);
		
					gl.depthFunc(gl.LEQUAL);
					gl.drawElements(gl.TRIANGLES, g_mesh.indices.numItems, gl.UNSIGNED_INT, 0);
	
					gl.cullFace(gl.BACK);
					gl.depthFunc(gl.ALWAYS);
					gl.drawElements(gl.TRIANGLES, g_mesh.indices.numItems, gl.UNSIGNED_INT, 0);
	
					gl.disable(gl.CULL_FACE);
					gl.depthFunc(gl.LEQUAL);
					gl.drawElements(gl.TRIANGLES, g_mesh.indices.numItems, gl.UNSIGNED_INT, 0);
				} else {
					/*	
					 *	Tri Pass wrong depth values
 			 		 */

					gl.depthFunc(gl.LEQUAL);
					gl.uniform1f(shaderProgram.alpha, 0.0);
					gl.drawElements(gl.TRIANGLES, g_mesh.indices.numItems, gl.UNSIGNED_INT, 0);
		
					gl.depthFunc(gl.GREATER);
					gl.uniform1f(shaderProgram.alpha, 0.0);
					gl.drawElements(gl.TRIANGLES, g_mesh.indices.numItems, gl.UNSIGNED_INT, 0);
		
					gl.depthFunc(gl.LEQUAL);
					gl.uniform1f(shaderProgram.alpha, 0.0);
					gl.drawElements(gl.TRIANGLES, g_mesh.indices.numItems, gl.UNSIGNED_INT, 0);
				}
			}
		}

		gl.flush();
		// end messure
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
		var data = muGl.canvas.toDataURL("image/png;base64;");
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


		var scene = g_playlist.scenes[g_scene_index];
		
		var frame_min = 0;
		var frame_max = scene.frames.length-1;

		var frame = scene.frames[0];
		var frameTo;
		
		mat4.perspective(pMatrix, muGl.degToRad(scene.aperture), gl.viewportWidth / gl.viewportHeight, frame.near, frame.far);
		mat4.lookAt(mvMatrix, frame.pos, frame.lookat, frame.up);
		mat4.translate(mvMatrix, mvMatrix, frame.lookat);
		mat4.multiply(invMvpMatrix, pMatrix, mvMatrix);
		mat4.invert(invMvpMatrix, invMvpMatrix);

		for(var i = frame_min; i <= frame_max; i++) {

			// messure time
			g_time_start = performance.now();

			frameTo = scene.frames[i];

			mat4.perspective(pMatrix, muGl.degToRad(scene.aperture), gl.viewportWidth / gl.viewportHeight, frameTo.near, frameTo.far);
			mat4.lookAt(mvMatrix, frameTo.pos, frameTo.lookat, frameTo.up);
			mat4.translate(mvMatrix, mvMatrix, frameTo.lookat);
			mat4.multiply(mvpMatrix, pMatrix, mvMatrix);
			mat4.multiply(resMvpMatrix, mvpMatrix, invMvpMatrix);
			
			drawEx();
	
			g_time_end = performance.now();

			saveFrameToPNG(i, g_time_end-g_time_start);
		}
	}

	function runEvaluation() {
		g_config	   = g_evaluator.getConfig(g_evaluation_id);
		g_callback_cnt = -3;

		updateConfig();
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
				initWebGl();
				getPlaylistMessage();

			};

			websocket.onmessage = function (evt) {
				var obj = JSON.parse(evt.data);
	
				if(typeof obj.result.scenes !== 'undefined') {
					playlist = obj.result;
					
					setPlaylist(playlist);
					
					//debug(JSON.stringify(playlist));
					debug("numScenes: " + playlist.scenes.length);
				}
				
				if(typeof obj.result.sceneReady !== 'undefined') {
					debug("test");
					setConfigMessage();
				}

				if(typeof obj.result.configReady !== 'undefined') {
					getFrameMessage(0);
				}

				if(typeof obj.result.rgb !== 'undefined') {
					muGl.setTextureFromBase64(colorTexture, g_config.textureCompressionMethod, obj.result.rgb, callback_draw);
				}
				
				if(typeof obj.result.depth !== 'undefined') {
					muGl.setTextureFromBase64(depthTexture, "png", obj.result.depth, callback_draw);
				}
				
				if(typeof obj.result.indices !== 'undefined') {
					var indices  = JSON.parse(obj.result.indices);
					var vertices = JSON.parse(obj.result.vertices);

					g_mesh.indices = new Uint32Array(indices);
					g_mesh.vertices = new Float32Array(vertices);
					g_mesh.texCoords = new Float32Array(obj.result.numTexCoord);

					callback_draw();
				}

				if(typeof obj.result.newMessureReady !== 'undefined') {
					runScene();
				}

				if(typeof obj.result.messureReady !== 'undefined') {
					g_evaluation_id = obj.result.messureReady + 1;

					$('#label_evaluation').text("    " + g_evaluation_id + " / " + g_evaluator.length + " | scene " + g_scene_index);
					
					$('#window_menu_overlay').width(100 * (g_evaluation_id / g_evaluator.length) + '%');

					if(g_evaluation_id < g_evaluator.length) {
						runEvaluation();
					} else  {
						g_scene_list_index++;
						g_config = new MuskatConfig(updateConfig);
						updateConfig();


						if(g_scene_list_index < g_scene_list.length) {
							debug("g_evaluation_id: " + g_scene_list_index + " " + g_scene_list[g_scene_list_index]);
							
							g_scene_index = g_scene_list[g_scene_list_index];
							setScene(g_playlist.scenes[g_scene_index]);
							loadSceneMessage(g_scene_index);

							getFrameMessage(0);
							
							$('#window_menu_overlay').width(0 + '%');
							g_evaluation_id = 0;
							runEvaluation();
						} else {
							g_evaluation_id = 0;
							alert("Evaluation Done!");
							$('#window_menu_overlay').width(0 + '%');
							$('fieldset').prop('disabled', false);
							$('legend').prop('disabled', false);
						}
					}
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

	function saveFrameMessage(id, base64, time) {
		var params = {
			"id" 	: id,
			"rgb" 	: base64,
			"time"	: time
		};
		sendMessage("saveFrame", params);
	}

	function getPlaylistMessage() {
		sendMessage("getPlaylist", {});
	}

	function loadSceneMessage(id) {
		idCnt = 0;
		
		var params = {"id" : id};
		sendMessage("loadScene", params);
	}

	function newMessureMessage(scene_id, messure_id, name, short_name) {
		var params = {
			"sceneId"		: scene_id,
			"messureId" 	: messure_id,
			"name" 			: name,
			"shortName"		: short_name,
			"numVertices"	: g_mesh.getNumVertices(),
			"numIndices"	: g_mesh.getNumIndices(),
			"numTriangles"	: g_mesh.getNumTriangles()
		};
		sendMessage("newMessure", params);
	}

	function setConfigMessage() {
		var params = {
			"width"						: g_config.width,
			"height"					: g_config.height,
			"meshWidth"					: g_config.meshWidth,
			"meshHeight"				: g_config.meshHeight,
			"smoothDepth"				: g_config.smoothDepth,
			"textureCompressionMethod" 	: g_config.textureCompressionMethod,
			"textureCompressionQuality"	: g_config.textureCompressionQuality,
			"meshMode"					: g_config.meshMode,
			"seedMode"					: g_config.seedMode,
			"gridType"					: g_config.gridType,
			"meshPrecision"				: g_config.meshPrecision,
			"meshCompression"			: g_config.meshCompression,
			"maxDepth"					: g_config.maxDepth,
			"Tleaf"						: g_config.Tleaf,
			"Tinternal"					: g_config.Tinternal,
			"Tangle"					: g_config.Tangle,
			"Tjoin"						: g_config.Tjoin,
			"Tthreshold"				: g_config.Tthreshold,
			"gamma"						: g_config.gamma,
			"refine"					: g_config.refine,
			"preBackgroundSubtraction"	: g_config.preBackgroundSubtraction,
			"praBackgroundSubtraction"	: g_config.praBackgroundSubtraction
		};

		debug(JSON.stringify(params));
		sendMessage("setConfig", params);
	}
});


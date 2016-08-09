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

	/*
	*	toggle visibility of the debug console
	*/

	$('#button_toggle_debug').click(function toggleDebug() {
		debug_console.style.visibility = (debug_console.style.visibility == "visible" || debug_console.style.visibility == "") ? "hidden" : "visible";
	});

	/*
	*	WebSocket global variables	
	*/

	var wsUri 		= "ws://localhost:1234";
	var websocket 	= null;
	var idCnt		= 0;

	/*
	*	change visibility of windows
	*/

	$('#button_connect_to_server').click(function connectToServer() {
		wsUri = $('#text_server_uri').val();	

		document.getElementById("window_config").style.display = "none";
		document.getElementById("window_render").style.display = "block";

		debug("connecting to " + wsUri);

		startWs();
	});

	/*
	*	Check if webgl is available and global variables
	*/

	if ( ! Detector.webgl ) Detector.addGetWebGLMessage();

	var display;
	var stats;
	var camera, scene, renderer;
	var uniforms;
	var texture_rgb;
	var texture;
	var mesh;
	var geometry;
	var material;
	var img;
	var depthImg;
	var canvas;
	var context;
	var theData;
	

	/*
	* WsFunctions
	*/

	function startWs() {
		try {
			if (typeof MozWebSocket == 'function')
				WebSocket = MozWebSocket;
			if ( websocket && websocket.readyState == 1 )
				websocket.close();

			websocket = new WebSocket( wsUri );
			websocket.onopen = function (evt) {
				debug("connected to " + wsUri);

				var test = new Person("test", "nicht", 2, "gray");
				debug(test.name(","));

				getPlaylistMessage();
				loadSceneMessage();
				initGl();
				animateGl();
			};

			websocket.onmessage = function (evt) {
				var obj = JSON.parse(evt.data);

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

	function initGl() {
		debug("init opengl");

		display = document.getElementById( 'display' );
		renderer = new THREE.WebGLRenderer();
		renderer.setSize(512, 512);
		//renderer.setPixelRatio( window.devicePixelRatio );		
		display.appendChild( renderer.domElement );

		scene = new THREE.Scene();

		camera = new THREE.PerspectiveCamera(45, renderer.domElement.width / renderer.domElement.height, 1, 10000);
		camera.position.set(0.0, 0.0, 10.0); 
		camera.lookAt(0.0, 0.0, 0.0);
		scene.add(camera);

		
		img = new Image();
		depthImg = new Image();

		canvas = document.createElement('canvas');
		context = canvas.getContext('2d');

		//var m3 = new THREE.Matrix4();
		//m3.set();
	

		geometry = new THREE.BufferGeometry();
		var vertices = new Float32Array( [
			-1.0, -1.0,  1.0,
			1.0, -1.0,  1.0,
			1.0,  1.0,  1.0,
					 
			1.0,  1.0,  1.0,
			-1.0,  1.0,  1.0,
			-1.0, -1.0,  1.0
		] );

		geometry.addAttribute( 'position', new THREE.BufferAttribute( vertices, 3 ) );

		var planeGeometry = new THREE.PlaneGeometry( 1, 1 );

		var uvTexture = new THREE.ImageUtils.loadTexture("img/UV_Grid_Sm.jpg"); 

		var planeMaterial = new THREE.MeshBasicMaterial({ 
			map:uvTexture, 
			side:THREE.DoubleSide 
		}); 
		
	

		mesh = new THREE.Mesh( planeGeometry, planeMaterial );
		mesh.position.set(0.0, 0.0, -1.0); 
		scene.add( mesh );

	
		debug("add stats");
		stats = new Stats();
		display.appendChild( stats.dom );

		onWindowResize();
		window.addEventListener( 'resize', onWindowResize, false );
		
		debug("opengl is ready");
		
	}

	function onWindowResize( event ) {
		camera = new THREE.PerspectiveCamera(45, renderer.domElement.width / renderer.domElement.height, 1, 100);
		debug("resize to "+ renderer.domElement.width + "X" + renderer.domElement.height);

		resizeMessage(renderer.domElement.width, renderer.domElement.height);	
	}

	function animateGl() {
		getFrameMessage();

		requestAnimationFrame( animateGl );
		render();
		stats.update();
	}

	function render() {
		renderer.render( scene, camera );
	}

	function resizeMessage(w, h) {
		var msg = {
			"jsonrpc" : "2.0",
			"method" : "resize",
			"params" : { 
				"width" : w, 
				"height": h
			},
			"id" : idCnt
		};
		debug(JSON.stringify(msg));
		websocket.send(JSON.stringify(msg));
		idCnt++;
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

	function createIndizies() {
	
	}
});


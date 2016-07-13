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
	var img;


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

				initGl();
				animateGl();
			};

			websocket.onmessage = function (evt) {
				var obj = JSON.parse(evt.data);

				if(obj.result.rgb != "") {
			
					texture = new THREE.Texture(img); 
					img.onload = function() {
						texture.needsUpdate = true;
						mesh.material.map = texture;
						mesh.material.needsUpdate = true;
					};
					img.src = 'data:image/jpeg;base64, ' + obj.result.rgb.toString();		
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
		renderer.setPixelRatio( window.devicePixelRatio );		
		display.appendChild( renderer.domElement );

		scene = new THREE.Scene();

		camera = new THREE.PerspectiveCamera(45, renderer.domElement.width / renderer.domElement.height, 1, 10000);
		camera.position.set(0.0, 0.0, 10.0); 
		camera.lookAt(0.0, 0.0, 0.0);
		scene.add(camera);

		
		img = new Image();

		var planeGeometry = new THREE.PlaneGeometry( 1, 1 );

		var boxGeometry = new THREE.BoxGeometry(1.0, 1.0, 1.0); 
		
		var uvTexture = new THREE.ImageUtils.loadTexture("img/UV_Grid_Sm.jpg"); 

		var planeMaterial = new THREE.MeshBasicMaterial({ 
			map:uvTexture, 
			side:THREE.DoubleSide 
		}); 
		
	

		mesh = new THREE.Mesh( planeGeometry, planeMaterial );
		mesh.position.set(0.0, 0.0, -5.0); 
		scene.add( mesh );

	
		debug("add stats");
		stats = new Stats();
		display.appendChild( stats.dom );

		onWindowResize();
		window.addEventListener( 'resize', onWindowResize, false );
		
		debug("opengl is ready");
		
	}

	function onWindowResize( event ) {
		renderer.setSize( window.innerWidth, window.innerHeight );
		camera = new THREE.PerspectiveCamera(45, renderer.domElement.width / renderer.domElement.height, 1, 100);
		debug("resize to "+ renderer.domElement.width + "X" + renderer.domElement.height);

		resizeMessage(renderer.domElement.width, renderer.domElement.height);	
	}

	function animateGl() {
	//	mesh.rotation.y += 0.1;
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
			"params" : {},
			"id" : idCnt
		};
		//debug(JSON.stringify(msg));
		websocket.send(JSON.stringify(msg));
		idCnt++;
	}
});

/*
*	muskat client v.0.1
*	Josef Schulz, 13.6.2016
*/

$(document).ready(function() {
	var debug_console = document.getElementById("textarea_debug");

	function debug(message) {
		debug_console.value += message + "\n";
		debug_console.scrollTop = debug_console.scrollHeight;
	}

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
				
				init();
				animate();
			};

			websocket.onmessage = function (evt) {
                //console.log( "Message received :", evt.data );
                debug( evt.data );
            };

			websocket.onclose = function (evt) {
				debug("disconnected");
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

	function init() {
		debug("init opengl");

		display = document.getElementById( 'display' );
		camera = new THREE.Camera();
		camera.position.z = 1;
		scene = new THREE.Scene();
		var geometry = new THREE.PlaneBufferGeometry( 2, 2, 1, 1 );
		uniforms = {
			time: { type: "f", value: 1.0 },
			texture: { type: "t", value: new THREE.TextureLoader().load("img/UV_Grid_Sm.jpg") },
			resolution: { type: "v2", value: new THREE.Vector2() }
		};
		var material = new THREE.ShaderMaterial( {
			uniforms: uniforms,
			vertexShader: document.getElementById( 'vertexShader' ).textContent,
			fragmentShader: document.getElementById( 'fragmentShader' ).textContent
		} );
		var mesh = new THREE.Mesh( geometry, material );
		scene.add( mesh );
		renderer = new THREE.WebGLRenderer();
		renderer.setPixelRatio( window.devicePixelRatio );
		display.appendChild( renderer.domElement );
		stats = new Stats();
		display.appendChild( stats.dom );
		onWindowResize();
		window.addEventListener( 'resize', onWindowResize, false );

		var controls = new THREE.FirstPersonControls( camera, renderer.domElement );
		controls.target.set( 0, 1, 0 );
		controls.update();

		debug("opengl is ready");
	}
	function onWindowResize( event ) {
		renderer.setSize( window.innerWidth, window.innerHeight );
		uniforms.resolution.value.x = renderer.domElement.width;
		uniforms.resolution.value.y = renderer.domElement.height;
		debug("resize");

		var msg = {
			"resize" : {
				"width" : renderer.domElement.width,
				"height" :  renderer.domElement.height
			}
		};

		websocket.send(JSON.stringify(msg));
	}

	function animate() {
		var msg = {
			"frame" : {
				camera : []
 			} 
		}
		requestAnimationFrame( animate );
		render();
		stats.update();
	}
	function render() {
		uniforms.time.value += 0.05;
		renderer.render( scene, camera );
	}
});

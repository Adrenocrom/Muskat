if ("WebSocket" in window) {
	// ws
	var ws = new WebSocket("ws://localhost:1234");

	ws.onerror = function() {
	    alert("error: " + ws);
	};

	ws.onopen = function() {
	    // Web Socket is connected, send data using send()
		alert("connection open: " + ws);
		ws.send("{\"width\":1033,\"height\":1002}")
	};

	ws.onmessage = function (evt) {
	    var received_msg = evt.data;		
	};

	ws.onclose = function() {
	    // websocket is closed.
		 alert("connection lost" + ws);
	};

	// opengl
	if ( ! Detector.webgl ) Detector.addGetWebGLMessage();
	var container, stats;
	var camera, scene, renderer;
	var uniforms;

	init();
	animate();

	function init() {
		container = document.getElementById( 'container' );
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
		container.appendChild( renderer.domElement );
		stats = new Stats();
		container.appendChild( stats.dom );
		onWindowResize();
		window.addEventListener( 'resize', onWindowResize, false );

		var controls = new THREE.FirstPersonControls( camera, renderer.domElement );
		controls.target.set( 0, 1, 0 );
		controls.update();

	}
	function onWindowResize( event ) {
		renderer.setSize( window.innerWidth, window.innerHeight );
		uniforms.resolution.value.x = renderer.domElement.width;
		uniforms.resolution.value.y = renderer.domElement.height;
	}
	//
	function animate() {
		requestAnimationFrame( animate );
		render();
		stats.update();
	}
	function render() {
		uniforms.time.value += 0.05;
		renderer.render( scene, camera );
	}
} else {
	alert("WebSocket NOT supported by your Browser!");
}

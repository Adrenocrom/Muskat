define("main", ["jqueryMin", 
					 "prototype", 
					 "init", 
					 "glMatrix-0.9.5.min",
					 "webgl-utils", 
					 "VEC2", 
					 "VEC3",
					 "JSONModel"], function() {

	shaderProgram = null;
//	boxShaderProg = null;
	function initShader() {
		var fragmentShader = getShader("scripts/fragment.glsl", "fragment");
		var vertexShader	 = getShader("scripts/vertex.glsl", "vertex");
/*
		var boxfragmentShader = getShader("scripts/boxfragment.glsl", "fragment");
		var boxvertexShader = getShader("scripts/boxvertex.glsl", "vertex");
*/
		shaderProgram = GlDevice.createProgram();
		GlDevice.attachShader(shaderProgram, vertexShader);
		GlDevice.attachShader(shaderProgram, fragmentShader);
		GlDevice.linkProgram(shaderProgram);
		if (!GlDevice.getProgramParameter(shaderProgram, GlDevice.LINK_STATUS))
            alert("Could not initialise shaders");
/*
		boxShaderProg = GlDevice.createProgram();
		GlDevice.attachShader(boxShaderProg, boxvertexShader);
		GlDevice.attachShader(boxShaderProg, boxfragmentShader);
		GlDevice.linkProgram(boxShaderProg);
		if (!GlDevice.getProgramParameter(boxShaderProg, GlDevice.LINK_STATUS))
				alert("Could not initialise boxShaders");
*/
		switchToShaderProgram();
   }

	function switchToShaderProgram() {
		GlDevice.useProgram(shaderProgram);

		shaderProgram.vertexPositionAttribute = GlDevice.getAttribLocation(shaderProgram, "aPosition");
      GlDevice.enableVertexAttribArray(shaderProgram.vertexPositionAttribute);
		shaderProgram.vertexNormalAttribute = GlDevice.getAttribLocation(shaderProgram, "aNormal");
		GlDevice.enableVertexAttribArray(shaderProgram.vertexNormalAttribute);
		shaderProgram.textureCoordAttribute = GlDevice.getAttribLocation(shaderProgram, "aTexCoord");
		GlDevice.enableVertexAttribArray(shaderProgram.textureCoordAttribute);
      shaderProgram.pMatrixUniform = GlDevice.getUniformLocation(shaderProgram, "uPMatrix");
      shaderProgram.mvMatrixUniform = GlDevice.getUniformLocation(shaderProgram, "uMVMatrix");
		shaderProgram.nMatrixUniform = GlDevice.getUniformLocation(shaderProgram, "uNMatrix");
		shaderProgram.samplerUniform = GlDevice.getUniformLocation(shaderProgram, "uSampler");
		shaderProgram.colorUniform = GlDevice.getUniformLocation(shaderProgram, "uColor");
		shaderProgram.positionLight = GlDevice.getUniformLocation(shaderProgram, "uPositionLight");
		shaderProgram.positionColor = GlDevice.getUniformLocation(shaderProgram, "uPositionColor");
		shaderProgram.ambientColor = GlDevice.getUniformLocation(shaderProgram, "uAmbientColor");
		//alert(shaderProgram.colorUniform);
	}

/*	function	switchToBoxShaderProgram() {
		GlDevice.useProgram(boxShaderProg);

		boxShaderProg.vertexPositionAttribute = GlDevice.getAttribLocation(boxShaderProg, "aPosition");
		GlDevice.enableVertexAttribArray(boxShaderProg.vertexPositionAttribute);
		boxShaderProg.pMatrixUniform = GlDevice.getUniformLocation(boxShaderProg, "uPMatrix");
		boxShaderProg.mvMatrixUniform = GlDevice.getUniformLocation(boxShaderProg, "uMVMatrix");
	}*/

	var Teapot;
	var Ship;
	var squareVertexPositionBuffer;
	var Stuka;
	var FW190;
	var Tisch;
	function initScene() {
		pushTexture("standart.png");
		pushTexture("house/tex1.png");
		pushTexture("house/tex2.png");
		pushTexture("house/tex3.png");
		pushTexture("house/tex4.png");
		pushTexture("house/tex5.png");
		pushTexture("house/tex6.png");
		pushTexture("fw190/mainBody.png");
		pushTexture("fw190/glass.png");
		pushTexture("fw190/controls.png");
		pushTexture("fw190/bomb1.png");
		pushTexture("fw190/bomb2.png");
		pushTexture("stuka/mainBody.png"); // 15
		pushTexture("stuka/under.png");
		pushTexture("stuka/glass1.png");
		pushTexture("stuka/cockpit.png");
		pushTexture("stuka/cockpit2.png");
		pushTexture("stuka/metal.png");
		pushTexture("stuka/metal.png");
		//pushTexture("boden-holz.jpg");

		Teapot = new JSONModel("Teapot.json");
		Ship = new JSONModel("house/bulding.json");
		Stuka = new JSONModel("stuka/stuka.json");
		FW190 = new JSONModel("fw190/FW190.json");
		//Tisch = new JSONModel("tisch.json");

		squareVertexPositionBuffer = GlDevice.createBuffer();
		GlDevice.bindBuffer(GlDevice.ARRAY_BUFFER, squareVertexPositionBuffer);
		vertices = [1.0,  1.0,  0.0,
						-1.0,  1.0,  0.0,
						1.0, -1.0,  0.0,
						-1.0, -1.0,  0.0];
		GlDevice.bufferData(GlDevice.ARRAY_BUFFER, new Float32Array(vertices), GlDevice.STATIC_DRAW);
		squareVertexPositionBuffer.itemSize = 3;
		squareVertexPositionBuffer.numItems = 4;
	}

	var mvMatrix = mat4.create();
	var pMatrix = mat4.create();

	function setMatrixUniforms() {
		GlDevice.uniformMatrix4fv(shaderProgram.pMatrixUniform, false, pMatrix);
		GlDevice.uniformMatrix4fv(shaderProgram.mvMatrixUniform, false, mvMatrix);

		var normalMatrix = mat3.create();
		mat4.toInverseMat3(mvMatrix, normalMatrix);
		mat3.transpose(normalMatrix);
		GlDevice.uniformMatrix3fv(shaderProgram.nMatrixUniform, false, normalMatrix);
	}

	var Angle = 180;
	var posZ = 30;
	function RenderScene() {
		GlDevice.viewport(0, 0, GlDevice.viewportWidth, GlDevice.viewportHeight);
      GlDevice.clear(GlDevice.COLOR_BUFFER_BIT | GlDevice.DEPTH_BUFFER_BIT);

		//alert(pMatrix);

      mat4.perspective(45, GlDevice.viewportWidth / GlDevice.viewportHeight, 0.1, 1000.0, pMatrix);
/*
		mat4.identity(mvMatrix);
		mat4.translate(mvMatrix, [3.0, 0.0, -7.0]);
		GlDevice.bindBuffer(GlDevice.ARRAY_BUFFER, squareVertexPositionBuffer);
		GlDevice.vertexAttribPointer(shaderProgram.vertexPositionAttribute, squareVertexPositionBuffer.itemSize, GlDevice.FLOAT, false, 0, 0);
		setMatrixUniforms();
		GlDevice.drawArrays(GlDevice.TRIANGLE_STRIP, 0, squareVertexPositionBuffer.numItems);*/

	/*	GlDevice.uniform3f(shaderProgram.positionColor, 1.0, 0.3, 0.3);
		GlDevice.uniform3f(shaderProgram.ambientColor, 0.6, 0.6, 0.6);
		GlDevice.uniform3f(shaderProgram.positionLight, 20.0, 1.0, -30.0);

      mat4.identity(mvMatrix);
      mat4.translate(mvMatrix, [-10.0, 0.0, -50.0]);
		mat4.rotate(mvMatrix, degToRad(23.4), [1, 0, -1]);
     	mat4.rotate(mvMatrix, degToRad(Angle), [0, 1, 0]);
      setMatrixUniforms();	
		Teapot.draw(0);
		Teapot.drawBoundingBox([1.0, 0.0, 0.0, 1.0]);
*/
		GlDevice.uniform3f(shaderProgram.positionColor, 0.0, 0.4, 0.0);
		GlDevice.uniform3f(shaderProgram.ambientColor, 0.8, 0.7, 0.9);
		GlDevice.uniform3f(shaderProgram.positionLight, 50.0, 1.0, 0.0);
/*
		mat4.identity(mvMatrix);
		mat4.translate(mvMatrix, [-10.0, -5.0, -30.0]);
		mat4.rotate(mvMatrix, degToRad(90.0), [-1, 0, 0]);
		mat4.scale(mvMatrix, [0.01, 0.01, 0.01]);
		setMatrixUniforms();
		Ship.draw([1,0,0,2,3,4,7]);
		Ship.drawBoundingBox([1.0, 1.0, 0.0, 1.0]);
*/		
		mat4.identity(mvMatrix);
		mat4.translate(mvMatrix, [0.0, -2.0, -20]);
		mat4.rotate(mvMatrix, degToRad(Angle), [0, 1, 0]);
		mat4.rotate(mvMatrix, degToRad(80.0), [-1, 0, 0]);
		mat4.scale(mvMatrix, [1.1, 1.1, 1.1]);
		setMatrixUniforms();
		FW190.draw([10, 11, 9, 8, 7]);
		FW190.drawBoundingBox([0.3, 0.3, 0.3, 1.0]);

/*
		mat4.identity(mvMatrix);
		mat4.translate(mvMatrix, [10.0, 0.0, -36.0]);
		mat4.rotate(mvMatrix, degToRad(Angle)/1.5, [0, 1, 0]);
		mat4.scale(mvMatrix, [1.1, 1.1, 1.1]);
		setMatrixUniforms();
		Stuka.draw([17, 13, 14, 15, 12, 12, 16]);//16]);
		Stuka.drawBoundingBox([0.0, 1.0, 0.0, 1.0]);	
/*		
		mat4.identity(mvMatrix);
		mat4.translate(mvMatrix, [0.0, -5.0, -16.0]);
		mat4.rotate(mvMatrix, degToRad(90.0), [-1, 0, 0]);
		mat4.rotate(mvMatrix, degToRad(30.0), [1, 0, 1]);
		mat4.scale(mvMatrix, [0.03, 0.03, 0.03]);
		setMatrixUniforms();
		Tisch.draw([1, 2]);
		Tisch.drawBoundingBox([0.0, 1.0, 1.0, 1.0]);
/*
		mat4.identity(mvMatrix);
		mat4.translate(mvMatrix, [-1.0, 0.0, posZ]);
		mat4.rotate(mvMatrix, degToRad(Angle)/0.5, [1, 0, 0]);
		setMatrixUniforms();
		Teapot.draw(1);
		Teapot.drawBoundingBox([0.0, 1.0, 0.0, 1.0]);*/
	}

	var globalTime = 0;
	var lastTime = 0;
	function MoveScene() {
		var timeNow = new Date().getTime();

		if(lastTime != 0) {
			var elapsed = timeNow - lastTime;

			Angle += 0.05 * elapsed;
			posZ -= 0.04 * elapsed;

			globalTime += elapsed;
		}

		lastTime = timeNow;
	}

	function doLoop() {
		requestAnimFrame(doLoop);
		RenderScene();
		MoveScene();
	}


	jQuery( document ).ready(function() {
		var canvas = document.getElementById("graphic_device");

		initGL(canvas);
		initShader();
		initScene();

		GlDevice.clearColor(0.0, 0.0, 0.0, 1.0);
		GlDevice.enable(GlDevice.DEPTH_TEST);

		doLoop();
	});
});

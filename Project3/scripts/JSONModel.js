JSONModel = Class.create({
	initialize: function(filename) {
		this.VBPosition;
		this.VBNormal;
		this.VBTextureCoord;
		this.IBIndizies = [];

		// Boundingbox
		this.AABBMin;
		this.AABBMax;

		this.VBBoundingBox;
		this.IBBoundingBox;
	
		var data = loadFileFromSrc(filename);//JSON.parse(request.responseText);

		data = JSON.parse(data);
			
		this.AABBMin = [Number.MAX_VALUE, Number.MAX_VALUE, Number.MAX_VALUE];
		this.AABBMax = [Number.MIN_VALUE, Number.MIN_VALUE, Number.MIN_VALUE];
				// this.AABBMax = vec3.create(Number.MIN_VALUE, Number.MIN_VALUE, Number.MIN_VALUE);
		
		this.VBNormal = GlDevice.createBuffer();
		GlDevice.bindBuffer(GlDevice.ARRAY_BUFFER, this.VBNormal);
		GlDevice.bufferData(GlDevice.ARRAY_BUFFER, new Float32Array(data.vertexNormals), GlDevice.STATIC_DRAW);
		this.VBNormal.itemSize = 3;
		this.VBNormal.numItems = data.vertexNormals.length / 3;

		this.VBTextureCoord = GlDevice.createBuffer();
		GlDevice.bindBuffer(GlDevice.ARRAY_BUFFER, this.VBTextureCoord);
		GlDevice.bufferData(GlDevice.ARRAY_BUFFER, new Float32Array(data.vertexTextureCoords), GlDevice.STATIC_DRAW);
		this.VBTextureCoord.itemSize = 2;
		this.VBTextureCoord.numItems = data.vertexTextureCoords.length / 2;

		this.VBPosition = GlDevice.createBuffer();
		GlDevice.bindBuffer(GlDevice.ARRAY_BUFFER, this.VBPosition);
		GlDevice.bufferData(GlDevice.ARRAY_BUFFER, new Float32Array(data.vertexPositions), GlDevice.STATIC_DRAW);
		this.VBPosition.itemSize = 3;
		this.VBPosition.numItems = data.vertexPositions.length / 3;

		// calculation of the bounding box
		// TODO:
		var k = 0;
		for(var i = 0; i < this.VBPosition.numItems; i++) {
				k = i*3;
				this.AABBMin[0] = Math.min(this.AABBMin[0], data.vertexPositions[k]);
				this.AABBMin[1] = Math.min(this.AABBMin[1], data.vertexPositions[k+1]);
				this.AABBMin[2] = Math.min(this.AABBMin[2], data.vertexPositions[k+2]);

				this.AABBMax[0] = Math.max(this.AABBMax[0], data.vertexPositions[k]);
				this.AABBMax[1] = Math.max(this.AABBMax[1], data.vertexPositions[k+1]);
				this.AABBMax[2] = Math.max(this.AABBMax[2], data.vertexPositions[k+2]);
		}

		for(var i = 0; i < data.indices.length; i++) {
			var indexBuffer = GlDevice.createBuffer();
			this.IBIndizies.push(indexBuffer);
			GlDevice.bindBuffer(GlDevice.ELEMENT_ARRAY_BUFFER, this.IBIndizies[i]);
			GlDevice.bufferData(GlDevice.ELEMENT_ARRAY_BUFFER, new Uint16Array(data.indices[i]), GlDevice.STATIC_DRAW);
			this.IBIndizies[i].itemSize = 1;
			this.IBIndizies[i].numItems = data.indices[i].length; 
		}
		/*
		this.IBIndizies = GlDevice.createBuffer();
		GlDevice.bindBuffer(GlDevice.ELEMENT_ARRAY_BUFFER, this.IBIndizies);
		GlDevice.bufferData(GlDevice.ELEMENT_ARRAY_BUFFER, new Uint16Array(data.indices), GlDevice.STATIC_DRAW);
		this.IBIndizies.itemSize = 1;
		this.IBIndizies.numItems = data.indices.length;
		*/
		// Create vertex and indexbuffer
		this.VBBoundingBox = GlDevice.createBuffer();
		GlDevice.bindBuffer(GlDevice.ARRAY_BUFFER, this.VBBoundingBox);
		var VerticeArrayBoundingBox = [this.AABBMin[0], this.AABBMin[1],this.AABBMin[2],
					 							 this.AABBMax[0], this.AABBMin[1],this.AABBMin[2],
				  	 							 this.AABBMax[0], this.AABBMax[1],this.AABBMin[2],
					 							 this.AABBMin[0], this.AABBMax[1],this.AABBMin[2],
					 							 this.AABBMin[0], this.AABBMin[1],this.AABBMax[2],
					 							 this.AABBMax[0], this.AABBMin[1],this.AABBMax[2],
					 							 this.AABBMax[0], this.AABBMax[1],this.AABBMax[2],
					 							 this.AABBMin[0], this.AABBMax[1],this.AABBMax[2]];
		GlDevice.bufferData(GlDevice.ARRAY_BUFFER, new Float32Array(VerticeArrayBoundingBox), GlDevice.STATIC_DRAW);
		this.VBBoundingBox.itemSize = 3;
		this.VBBoundingBox.numItems = 3;

		this.IBBoundingBox = GlDevice.createBuffer();
		GlDevice.bindBuffer(GlDevice.ELEMENT_ARRAY_BUFFER, this.IBBoundingBox);
		var IndexBufferArray = [0, 1, 1, 2, 2, 3, 3, 0, // front
										0, 4, 4, 7, 7, 3, 3, 0, // left
										3, 2, 2, 6, 6, 7, 7, 3, // top
										0, 1, 1, 5, 5, 4, 4, 0, // bottom
										1, 5, 5, 6, 6, 2, 2, 1, // right
										4, 5, 5, 6, 6, 7, 7, 4]; // back
		GlDevice.bufferData(GlDevice.ELEMENT_ARRAY_BUFFER, new Uint16Array(IndexBufferArray), GlDevice.STATIC_DRAW);
		this.IBBoundingBox.itemSize = 1;
		this.IBBoundingBox.numItems = 48;
	},
	draw: function(textureIds) {
		GlDevice.uniform4fv(shaderProgram.colorUniform, new Float32Array([0.0, 0.0, 0.0, 0.0]));
		GlDevice.bindBuffer(GlDevice.ARRAY_BUFFER, this.VBPosition);
		GlDevice.vertexAttribPointer(shaderProgram.vertexPositionAttribute, this.VBPosition.itemSize, GlDevice.FLOAT, false, 0, 0);

		GlDevice.bindBuffer(GlDevice.ARRAY_BUFFER, this.VBTextureCoord);
		GlDevice.vertexAttribPointer(shaderProgram.textureCoordAttribute, this.VBTextureCoord.itemSize, GlDevice.FLOAT, false, 0, 0);

		GlDevice.bindBuffer(GlDevice.ARRAY_BUFFER, this.VBNormal);
		GlDevice.vertexAttribPointer(shaderProgram.vertexNormalAttribute, this.VBNormal.itemSize, GlDevice.FLOAT, false, 0, 0);
/*
		GlDevice.activeTexture(GlDevice.TEXTURE0);
		GlDevice.bindTexture(GlDevice.TEXTURE_2D, globalTextureStack[textureId]);
		GlDevice.uniform1i(shaderProgram.samplerUniform, 0);
*/		
		for(var i = 0; i < this.IBIndizies.length; i++) {
			GlDevice.activeTexture(GlDevice.TEXTURE0);
			GlDevice.bindTexture(GlDevice.TEXTURE_2D, globalTextureStack[textureIds[i]]);
			GlDevice.uniform1i(shaderProgram.samplerUniform, 0);

			GlDevice.bindBuffer(GlDevice.ELEMENT_ARRAY_BUFFER, this.IBIndizies[i]);
			GlDevice.drawElements(GlDevice.TRIANGLES, this.IBIndizies[i].numItems, GlDevice.UNSIGNED_SHORT, 0);
		}
	},
	drawBoundingBox: function(color) {
		GlDevice.uniform4fv(shaderProgram.colorUniform, new Float32Array(color));
		//GlDevice.useProgram(boxShaderProg);
		GlDevice.bindBuffer(GlDevice.ARRAY_BUFFER, this.VBBoundingBox);
		GlDevice.vertexAttribPointer(shaderProgram.vertexPositionAttribute, this.VBBoundingBox.itemSize, GlDevice.FLOAT, false, 0, 0);
		GlDevice.bindBuffer(GlDevice.ELEMENT_ARRAY_BUFFER, this.IBBoundingBox);
		GlDevice.drawElements(GlDevice.LINES, this.IBBoundingBox.numItems, GlDevice.UNSIGNED_SHORT, 0);
		//GlDevice.useProgram(shaderProgram);
	}
});

class MuskatConfig {
	constructor(callback) {
		this._textureCompressionMethod 	= "jpeg";
		this._textureCompressionQuality = 0;
		this._meshCompressionMethod 	= "8bit";
		this._meshCompressionQualtiy	= 0;
		this._callback = callback;
	}

	get textureCompressionMethod() {
		return this._textureCompressionMethod;
	}

	get textureCompressionQuality() {
		return this._textureCompressionQuality;
	}

	get meshCompressionMethod() {
		return this._meshCompressionMethod;
	}

	get meshCompressionQualtiy() {
		return this._meshCompressionQualtiy;
	}

	set textureCompressionMethod(method) {
		this._textureCompressionMethod = method;
		this._callback();
	}

	set textureCompressionQuality(quality) {
		this._textureCompressionQuality = quality;
		this._callback();
	}

	set meshCompressionMethod(method) {
		this._meshCompressionMethod = method;
		this._callback();
	}

	set meshCompressionQualtiy(quality) {
		this._meshCompressionQualtiy = quality;
		this._callback();
	}
}

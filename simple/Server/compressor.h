#ifndef COMPRESSOR_H
#define COMPRESSOR_H

#pragma once

class Config;

class Compressor {
public:

	Compressor(Config* config);

	QJsonObject compressFrame(FrameInfo& info, FrameBuffer& fb);
private:

	Config* m_config;

	void compressTexture(QJsonObject& jo, FrameBuffer& fb);
	void compressMesh(QJsonObject& jo, FrameBuffer& fb);

	void compressMesh8Bit(QJsonObject& jo, FrameBuffer& fb);
	void compressMesh16Bit(QJsonObject& jo, FrameBuffer& fb);
	void compressMeshDelaunay(QJsonObject& jo, FrameBuffer& fb);
};

#endif

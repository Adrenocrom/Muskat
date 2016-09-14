#include "muskat.h"

Config::Config() {
	textureCompressionMethod 	= "jpeg";
	textureCompressionQuality 	= 0;
	meshCompressionMethod		= "8bit";
	meshCompressionQuality		= 0;

	m_width 	= 512;
	m_height 	= 512;
	m_asprect 	= 1.0f;

	m_T_leaf 	 = 0;
	m_T_internal = 0;

	// default creates just a plane
	m_max_depth	 = 1;
}

void Config::setConfig(QJsonObject& jo) {
	textureCompressionMethod 	= jo["textureCompressionMethod"].toString();
	textureCompressionQuality	= jo["textureCompressionQuality"].toInt();
	meshCompressionMethod		= jo["meshCompressionMethod"].toString();
	meshCompressionQuality		= jo["meshCompressionQuality"].toInt();
}


QJsonObject Config::getConfig() {
	QJsonObject jo;

	jo["textureCompressionMethod"] 	= textureCompressionMethod;
	jo["textureCompressionQuality"]	= textureCompressionQuality;
	jo["meshCompressionMethod"]		= meshCompressionMethod;
	jo["meshCompressionQuality"]	= meshCompressionQuality;

	return jo;
}

int Config::getWidth() {
	return m_width;
}

int Config::getHeight() {
	return m_height;
}

float Config::getAspect() {
	m_asprect = (float) m_width / (float) m_height;
	return m_asprect;
}

ushort Config::getTleaf() {
	return m_T_leaf;
}

ushort Config::getTinternal() {
	return m_T_internal;
}

uint Config::getMaxDepth() {
	return m_max_depth;
}

#include "muskat.h"

Config::Config() {
	m_width 	= 512;
	m_height 	= 512;
	m_asprect 	= 1.0f;

	m_textureCompressionMethod 	= "jpeg";
	m_textureCompressionQuality = 0;

	m_mesh_mode = "full";
	m_grid_type = "default";
	m_mesh_percesion = "8bit";

	m_mesh_compression = 1;

	m_T_leaf 	 = 0;
	m_T_internal = 0;

	// default creates just a plane
	m_max_depth	 = 1;
}

void Config::setConfig(QJsonObject& jo) {
	m_width		= jo["width"].toInt();
	m_height	= jo["height"].toInt();
	m_textureCompressionMethod 	= jo["textureCompressionMethod"].toString();
	m_textureCompressionQuality	= jo["textureCompressionQuality"].toInt();
	m_mesh_mode 		= jo["meshMode"].toString();
	m_grid_type 		= jo["gridType"].toString();
	m_mesh_percesion 	= jo["meshPercesion"].toString();
	m_mesh_compression 	= jo["meshCompression"].toInt();
	m_max_depth 		= (uint) jo["maxDepth"].toInt();
	m_T_leaf 			= (ushort)((float)jo["Tleaf"].toDouble() * (float)(USHRT_MAX));
	m_T_internal 		= (ushort)((float)jo["Tinternal"].toDouble() * (float)(USHRT_MAX));
}


QJsonObject Config::getConfig() {
	QJsonObject jo;
	jo["width"] 	= m_width;
	jo["height"] 	= m_height;
	jo["textureCompressionMethod"] 	= m_textureCompressionMethod;
	jo["textureCompressionQuality"]	= m_textureCompressionQuality;
	jo["meshMode"]	= m_mesh_mode;
	jo["gridType"]	= m_grid_type;
	jo["meshPercesion"] 	= m_mesh_percesion;
	jo["meshCompression"] 	= m_mesh_compression;
	jo["maxDepth"]	= (int) m_max_depth;
	jo["Tleaf"]  	= (double) m_T_leaf / (double) (USHRT_MAX);
	jo["Tinternal"] = (double) m_T_internal / (double) (USHRT_MAX);
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

QString Config::getTextureCompressionMethod() {
	return m_textureCompressionMethod;
}

int Config::getTextureCompressionQuality() {
	return m_textureCompressionQuality;
}

QString Config::getMeshMode() {
	return m_mesh_mode;
}

QString Config::getGridType() {
	return m_grid_type;
}

QString Config::getMeshPercesion() {
	return m_mesh_percesion;
}

int Config::getMeshCompression() {
	return m_mesh_compression;
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

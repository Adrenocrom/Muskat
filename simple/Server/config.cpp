#include "muskat.h"

Config::Config(MainWindow* mw) {
	m_mw = mw;

	m_width 	= 512;
	m_height 	= 512;
	m_asprect 	= 1.0f;

	m_mesh_width 	= 512;
	m_mesh_height 	= 512;

	m_textureCompressionMethod 	= "jpeg";
	m_textureCompressionQuality = 0;

	m_mesh_mode = "full";
	m_grid_type = "default";
	m_mesh_percesion = "8bit";

	m_mesh_compression = 1;

	m_T_leaf 	 = 0;
	m_T_internal = 0;
	m_T_angle	 = 0.2;
	m_T_join	 = 0.5;

	// default creates just a plane
	m_max_depth	 = 8;

	m_use_background = false;
}

void Config::setConfig(QJsonObject& jo) {
	uint maxDepth = (uint) jo["maxDepth"].toInt();
	int mesh_width	= jo["meshWidth"].toInt();
	int mesh_height	= jo["meshHeight"].toInt();

	m_width		= jo["width"].toInt();
	m_height	= jo["height"].toInt();
	m_textureCompressionMethod 	= jo["textureCompressionMethod"].toString();
	m_textureCompressionQuality	= jo["textureCompressionQuality"].toInt();
	m_mesh_mode 		= jo["meshMode"].toString();
	m_grid_type 		= jo["gridType"].toString();
	m_mesh_percesion 	= jo["meshPercesion"].toString();
	m_mesh_compression 	= jo["meshCompression"].toInt();
	m_T_leaf 			= (ushort)((float)jo["Tleaf"].toDouble() * (float)(USHRT_MAX));
	m_T_internal 		= (ushort)((float)jo["Tinternal"].toDouble() * (float)(USHRT_MAX));
	m_T_angle			= jo["Tangle"].toDouble();
	m_T_join			= jo["Tjoin"].toDouble();
	m_use_background	= jo["useBackground"].toBool();

	if(maxDepth != m_max_depth || mesh_width != m_mesh_width || mesh_height != m_mesh_height) {
		m_max_depth 	= maxDepth;
		m_mesh_width	= mesh_width;
		m_mesh_height	= mesh_height;
		m_mw->m_compressor->resizeQuadtree();
	}
}


QJsonObject Config::getConfig() {
	QJsonObject jo;
	jo["width"] 	= m_width;
	jo["height"] 	= m_height;
	jo["meshWidth"] = m_mesh_width;
	jo["meshHeight"]= m_mesh_height;
	jo["textureCompressionMethod"] 	= m_textureCompressionMethod;
	jo["textureCompressionQuality"]	= m_textureCompressionQuality;
	jo["meshMode"]	= m_mesh_mode;
	jo["gridType"]	= m_grid_type;
	jo["meshPercesion"] 	= m_mesh_percesion;
	jo["meshCompression"] 	= m_mesh_compression;
	jo["maxDepth"]	= (int) m_max_depth;
	jo["Tleaf"]  	= (double) m_T_leaf / (double) (USHRT_MAX);
	jo["Tinternal"] = (double) m_T_internal / (double) (USHRT_MAX);
	jo["Tangle"]	= m_T_angle;
	jo["Tjoin"]		= m_T_join;
	jo["useBackground"] = m_use_background;
	return jo;
}

bool Config::hasDifferentSize() {
	if(m_width != m_mesh_width || m_height != m_mesh_height)
		return true;
	return false;
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

int Config::getMeshWidth() {
	return m_mesh_width;
}

int Config::getMeshHeight() {
	return m_mesh_height;
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

bool Config::getUseBackground() {
	return m_use_background;
}

double Config::getTangle() {
	return m_T_angle;
}

double Config::getTjoin() {
	return m_T_join;
}

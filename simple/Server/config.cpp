/***********************************************************
 *
 *
 *						CONFIG SOURCE
 *					 ===================
 *
 *		AUTHOR: Josef Schulz
 *
 *		Implements getter, setter and the jsonobject 
 *		parse methods.
 *
 ***********************************************************/


#include "muskat.h"

// create default config
Config::Config(MainWindow* mw) {
	m_mw = mw;

	m_width 	= 512;
	m_height 	= 512;
	m_asprect 	= 1.0f;

	m_mesh_width 	= 512;
	m_mesh_height 	= 512;

	m_smooth_depth	= false;

	m_textureCompressionMethod 	= "jpeg";
	m_textureCompressionQuality = 0;

	m_mesh_mode = "full";
	m_seed_mode	= "quadtree";
	m_grid_type = "default";
	m_mesh_precision = "8bit";

	m_mesh_compression = 1;

	m_T_leaf 	 = 0;
	m_T_internal = 0;
	m_T_angle	 = 0.2;
	m_T_join	 = 0.5;

	// default creates just a plane
	m_max_depth	 = 8;

	m_T_threshold = 0.5;
	m_gamma		  = 0.5;

	m_refine	 = true;

	m_pre_background_subtraction = false;
	m_pra_background_subtraction = false;
}

// recived config will be used, parsed from jsonobject
void Config::setConfig(QJsonObject& jo) {
	uint maxDepth = (uint) jo["maxDepth"].toInt();
	int mesh_width	= jo["meshWidth"].toInt();
	int mesh_height	= jo["meshHeight"].toInt();

	m_width		= jo["width"].toInt();
	m_height	= jo["height"].toInt();
	m_smooth_depth = jo["smoothDepth"].toBool();
	m_textureCompressionMethod 	= jo["textureCompressionMethod"].toString();
	m_textureCompressionQuality	= jo["textureCompressionQuality"].toInt();
	m_mesh_mode 		= jo["meshMode"].toString();
	m_seed_mode			= jo["seedMode"].toString();
	m_grid_type 		= jo["gridType"].toString();
	m_mesh_precision 	= jo["meshPrecision"].toString();
	m_mesh_compression 	= jo["meshCompression"].toInt();
	m_T_leaf 			= (ushort)(jo["Tleaf"].toDouble()     * (double)(SHRT_MAX));
	m_T_internal 		= (ushort)(jo["Tinternal"].toDouble() * (double)(SHRT_MAX));
	m_T_angle			= jo["Tangle"].toDouble();
	m_T_join			= jo["Tjoin"].toDouble();
	m_T_threshold		= jo["Tthreshold"].toDouble();
	m_gamma				= jo["gamma"].toDouble();
	m_refine			= jo["refine"].toBool();
	m_pre_background_subtraction = jo["preBackgroundSubtraction"].toBool();
	m_pra_background_subtraction = jo["praBackgroundSubtraction"].toBool();

	// if the maximum depth of the quadtree is changed
	// or the depth image size has changed the quadtree have to resized
	if(maxDepth != m_max_depth || mesh_width != m_mesh_width || mesh_height != m_mesh_height) {
		m_max_depth 	= maxDepth;
		m_mesh_width	= mesh_width;
		m_mesh_height	= mesh_height;
		m_mw->m_compressor->resizeQuadtree();
	}
}

// create json object from config
QJsonObject Config::getConfig() {
	QJsonObject jo;
	jo["width"] 	= m_width;
	jo["height"] 	= m_height;
	jo["meshWidth"] = m_mesh_width;
	jo["meshHeight"]= m_mesh_height;
	jo["smoothDepth"] = m_smooth_depth;
	jo["textureCompressionMethod"] 	= m_textureCompressionMethod;
	jo["textureCompressionQuality"]	= m_textureCompressionQuality;
	jo["meshMode"]	= m_mesh_mode;
	jo["seedMode"]  = m_seed_mode;
	jo["gridType"]	= m_grid_type;
	jo["meshPrecision"] 	= m_mesh_precision;
	jo["meshCompression"] 	= m_mesh_compression;
	jo["maxDepth"]	= (int) m_max_depth;
	jo["Tleaf"]		= (double) m_T_leaf 	/ (double) (SHRT_MAX);
	jo["Tinternal"] = (double) m_T_internal / (double) (SHRT_MAX);
	jo["Tangle"]	= m_T_angle;
	jo["Tjoin"]		= m_T_join;
	jo["Tthreshold"]= m_T_threshold;
	jo["gamma"]		= m_gamma;
	jo["refine"]	= m_refine;
	jo["preBackgroundSubtraction"] = m_pre_background_subtraction;
	jo["praBackgroundSubtraction"] = m_pra_background_subtraction;
	return jo;
}

// check if the size of rgb and depth image are not equal
bool Config::hasDifferentSize() {
	if(m_width != m_mesh_width || m_height != m_mesh_height) {
		cout<<m_mesh_width<<" "<<m_mesh_height<<endl;
		return true;
	}
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

bool Config::getSmoothDepth() {
	return m_smooth_depth;
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

QString Config::getSeedMode() {
	return m_seed_mode;
}

QString Config::getGridType() {
	return m_grid_type;
}

QString Config::getMeshPrecision() {
	return m_mesh_precision;
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

bool Config::preBackgroundSubtraction() {
	return m_pre_background_subtraction;
}

bool Config::praBackgroundSubtraction() {
	return m_pra_background_subtraction;
}

double Config::getTangle() {
	return m_T_angle;
}

double Config::getTjoin() {
	return m_T_join;
}

double Config::getTthreshold() {
	return m_T_threshold;
}

double Config::getGamma() {
	return m_gamma;
}

bool Config::getRefine() {
	return m_refine;
}

bool Config::useDelaunay() {
	if(m_mesh_mode == QString("delaunay"))
		return true;
	return false;
}

bool Config::useQuadtree() {
	if(m_seed_mode == QString("quadtree"))
		return true;
	return false;
}

#ifndef CONFIG_H
#define CONFIG_H

#pragma once

class Config {
public:



	// constructor set up defaults
	Config();

	// produces json object for transfer via jsonrpc
	QJsonObject getConfig();

	// parse settings from recived json object
	void	setConfig(QJsonObject& jo);


	// screen  getter
	int		getWidth();
	int		getHeight();
	float	getAspect();

	QString	getTextureCompressionMethod();
	int		getTextureCompressionQuality();

	QString	getMeshMode();
	QString getGridType();
	QString getMeshPercesion();

	int		getMeshCompression();

	// quadtree getter
	uint	getMaxDepth();

	// delaunay threshold getter
	ushort	getTleaf();
	ushort	getTinternal();

private:

	// screen settings
	int 	m_width;		// screen-width
	int 	m_height;		// screen-height
	float 	m_asprect;		// aspect ratio calculated while resize

	QString		m_textureCompressionMethod;
	int			m_textureCompressionQuality;

	QString		m_mesh_mode;
	QString		m_grid_type;
	QString		m_mesh_percesion;

	int			m_mesh_compression;

	// thresholds for delaunay triangulation
	ushort	m_T_leaf;
	ushort	m_T_internal;

	// quadtree config
	uint 	m_max_depth;	// max depth
};

#endif

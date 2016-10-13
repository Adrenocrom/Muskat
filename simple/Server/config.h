#ifndef CONFIG_H
#define CONFIG_H

#pragma once

class MainWindow;

class Config {
public:

	// constructor set up defaults
	Config(MainWindow* mw);

	// produces json object for transfer via jsonrpc
	QJsonObject getConfig();

	// parse settings from recived json object
	void	setConfig(QJsonObject& jo);


	// screen  getter
	int		getWidth();
	int		getHeight();
	float	getAspect();

	int		getMeshWidth();
	int		getMeshHeight();

	bool	getSmoothDepth();

	bool	hasDifferentSize();

	QString	getTextureCompressionMethod();
	int		getTextureCompressionQuality();

	QString	getMeshMode();
	QString getGridType();
	QString getMeshPercesion();

	int		getMeshCompression();
	bool	useDelaunay();

	// quadtree getter
	uint	getMaxDepth();

	// delaunay threshold getter
	ushort	getTleaf();
	ushort	getTinternal();

	double	getTangle();
	double	getTjoin();

	bool	getRefine();

	bool	preBackgroundSubtraction();
	bool	praBackgroundSubtraction();

private:
	// pointer to mainwindow
	MainWindow* m_mw;

	// screen settings
	int 	m_width;		// screen-width
	int 	m_height;		// screen-height
	float 	m_asprect;		// aspect ratio calculated while resize

	int		m_mesh_width;
	int		m_mesh_height;

	bool	m_smooth_depth;

	QString		m_textureCompressionMethod;
	int			m_textureCompressionQuality;

	QString		m_mesh_mode;
	QString		m_grid_type;
	QString		m_mesh_percesion;

	int			m_mesh_compression;

	// thresholds for delaunay triangulation
	ushort	m_T_leaf;
	ushort	m_T_internal;

	double	m_T_angle;
	double	m_T_join;

	// quadtree config
	uint 	m_max_depth;	// max depth

	// do refinement step
	bool	m_refine;

	// delete background triangles, if low max depth produces errors
	bool	m_pre_background_subtraction;
	bool	m_pra_background_subtraction;
};

#endif

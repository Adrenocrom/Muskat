/***********************************************************
 *
 *
 *						CONFIG HEADER
 *					 ===================
 *
 *		AUTHOR: Josef Schulz
 *
 *		In this class the config settings are stored.
 *		Methods to create and set from or to a 
 *		json object are implemented.
 *
 ***********************************************************/

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

	// the depth image size from the rgb image
	int		getMeshWidth();
	int		getMeshHeight();

	// the depth image can be smoothed
	bool	getSmoothDepth();

	// if rgb and depth image size are different this method return true
	bool	hasDifferentSize();

	// texture compression could be done with JPEG or PNG
	QString	getTextureCompressionMethod();
	int		getTextureCompressionQuality(); // JPEG 0-100, PNG 0-9

	QString	getMeshMode();		// full or delaunay
	QString	getSeedMode();		// quadtree or floydSteinberg (delaunay)
	QString getGridType();	    // default, coockie-cutter or isometrisch (full)
	QString getMeshPrecision(); // 8 or 16 bit (full)

	int		getMeshCompression(); // (FULL) depth image compressed with png
	
	bool	useDelaunay(); 
	bool	useQuadtree();		  

	// quadtree getter
	uint	getMaxDepth();

	// delaunay threshold getter
	ushort	getTleaf();
	ushort	getTinternal();

	// refinement params
	double	getTangle();
	double	getTjoin();

	// floydSteinberg params
	double  getTthreshold();
	double  getGamma();

	// do refine
	bool	getRefine();

	// discard seed points which are part of the background
	bool	preBackgroundSubtraction(); 
	
	// after the mesh is created discard triangles, when all vertices
	// are part of the background (does not work with floydSteinberg, or with pre backgroud filter)
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
	QString		m_seed_mode;
	QString		m_grid_type;
	QString		m_mesh_precision;

	int			m_mesh_compression;

	// thresholds for delaunay triangulation
	ushort	m_T_leaf;
	ushort	m_T_internal;

	double	m_T_angle;
	double	m_T_join;

	// quadtree config
	uint 	m_max_depth;	// max depth

	// floyd steinberg
	double	m_T_threshold;
	double	m_gamma;

	// do refinement step
	bool	m_refine;

	// delete background triangles, if low max depth produces errors
	bool	m_pre_background_subtraction;
	bool	m_pra_background_subtraction;
};

#endif

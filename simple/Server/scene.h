/***********************************************************
 *
 *
 *						SCENE HEADER
 *					 ==================
 *
 *		AUTHOR: Josef Schulz
 *
 *		A scene represent a sequence of tripples.
 *		Each tripple consits of and rgb image,
 *		depth image and camera-informations.
 *
 *		FrameInfo: Camera information.
 *		FrameBuffer: rgb image, depth image. (Wrapper class)
 *
 * 		Both are declared in the fileRenderer.h
 *
 ***********************************************************/


#ifndef SCENE_H
#define SCENE_H

#pragma once

class Scene {
public:
	string 		m_scene_name;
	string		m_filename;
	string		m_suffix;
	uint		m_timestep;
	uint		m_height;
	uint		m_width;
	QVector3D	m_aabb_min;
	QVector3D	m_aabb_max;
	double		m_aspect;
	double		m_aperture;

	vector<FrameInfo> 		m_infos;	// stores the frame infos of the sequenz
	vector<FrameBuffer> 	m_fbs;		// stores rgb and depth images of the sequenz

	// loads a scene from disk
	Scene(string filename, string suffix);

	// create an empty scene
	Scene(string name);

	// creates a jsonobject-representation of the frame informations
	QJsonObject	getSceneInfo();

	// combine two scenes, to create one sequence of 3 runs
	void addScene(Scene scene);

private:
	
	// creates an json object from frame info, needed for the playlist
	QJsonObject getFrameInfo(uint frame_id);

	// reads a line of the scene info file
	void 		procressLine(stringstream& line, FrameInfo& info);

	// removes some unneeded chars from a string
	QString 	removeChars(string& str);
};

#endif

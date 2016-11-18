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

	vector<FrameInfo> 		m_infos;
	vector<FrameBuffer> 	m_fbs;

	Scene(string filename, string suffix);
	Scene(string name);

	QJsonObject	getSceneInfo();

	void addScene(Scene scene);

private:
	QJsonObject getFrameInfo(uint frame_id);
	void 		procressLine(stringstream& line, FrameInfo& info);
	QString 	removeChars(string& str);
};

#endif

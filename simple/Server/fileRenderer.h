#ifndef FILERENDERER_H
#define FILERENDERER_H

#pragma once

struct SFrameInfo;

struct SFrameBuffer;

class FileRenderer : public AbstractRenderer {
	public:
		string 					m_filename;
		uint						m_timestep;
		QVector3D				m_boundingbox_min;
		QVector3D				m_boundingbox_max;
		uint						m_width;
		uint						m_height;
		float						m_aspect;
		float						m_aperture;

		QMatrix4x4				m_camera;
		QMatrix4x4				m_projection;

		vector<SFrameInfo> 	m_infos;
		vector<SFrameBuffer> m_fbs;

		void getFrame(SFrameInfo& info, SFrameBuffer& fb);
		void loadScene(string filename, string suffix);
		int getMaxFrames() {return m_infos.size();}
	
	private:
		void procressLine(stringstream& line);
		string removeChars(string& str);
};

#endif

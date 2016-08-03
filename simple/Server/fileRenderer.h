#ifndef FILERENDERER_H
#define FILERENDERER_H

#pragma once

struct SFrameInfo {
	uint 		id;
	QVector3D 	pos;
	QVector3D 	lookAt;
	QVector3D 	up;
	float		near;
	float 		far;
	float 		offangle;
};

struct SFrameBuffer {
	cv::Mat rgb;
	cv::Mat depth;
};

class Scene;

class FileRenderer {
	public:
		void getFrame(SFrameInfo& info, SFrameBuffer& fb);

		void setScene(Scene* pScene);
		
	private:
		Scene*	m_pScene;
};

#endif

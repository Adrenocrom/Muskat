#ifndef FILERENDERER_H
#define FILERENDERER_H

#pragma once

struct FrameInfo {
	uint 		id;
	QVector3D 	pos;
	QVector3D 	lookAt;
	QVector3D 	up;
	float		near;
	float 		far;
	float 		offangle;
};

struct FrameBuffer {
	cv::Mat rgb;
	cv::Mat depth;
};

class Scene;

class FileRenderer {
	public:
		void getFrame(FrameInfo& info, FrameBuffer& fb);

		void setScene(Scene* pScene);
		
	private:
		Scene*	m_pScene;
};

#endif

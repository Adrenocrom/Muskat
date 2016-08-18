#ifndef FILERENDERER_H
#define FILERENDERER_H

#pragma once

struct FrameInfo {
	uint 		id;
	QVector3D 	pos;
	QVector3D 	lookAt;
	QVector3D 	up;
	double		near;
	double 		far;
	double 		offangle;

	FrameInfo() {
		id = -1;
	}

	void out() {
		cout<<"frame: "<<id<<endl;
		cout<<pos.x()<<", "<<pos.y()<<", "<<pos.z()<<endl;
		cout<<lookAt.x()<<", "<<lookAt.y()<<", "<<lookAt.z()<<endl;
		cout<<up.x()<<", "<<up.y()<<", "<<up.z()<<endl;
		cout<<near<<" / "<<far<<endl;
		cout<<offangle<<endl;
	}
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
		Scene* getScene() {return m_pScene;}
		
	private:
		Scene*	m_pScene;
};

#endif

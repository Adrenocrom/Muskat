/***********************************************************
 *
 *
 *						FILERENDERER HEADER
 *					 =========================
 *
 *		AUTHOR: Josef Schulz
 *
 * 		Demo Renderer uses the loaded sequences as an
 * 		response.
 *
 ***********************************************************/

#ifndef FILERENDERER_H
#define FILERENDERER_H

#pragma once

// stores camera information, loaded from the scenes 
// or recevied by the client.
struct FrameInfo {
	uint 		id;			// used for sequence based scenes
	QVector3D 	pos;		// camera position
	QVector3D 	lookAt;		// the camera is looking at this point
	QVector3D 	up;			// needed to define whats the top
	double		near;		// near clipping plane 
	double 		far;		// far	clipping plane
	double 		offangle;	// actually the opening angle, or aperture angle

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

// frame buffer, output created by the renderer
struct FrameBuffer {
	cv::Mat rgb;
	cv::Mat depth;
};

class Scene;

class FileRenderer {
	public:
		
		// creates frame by frame info
		void getFrame(FrameInfo& info, FrameBuffer& fb);

		// load new scene, or just replace the pointer
		void setScene(Scene* pScene);
		Scene* getScene() {return m_pScene;}
		
	private:

		// actual loaded scene
		Scene*	m_pScene;
};

#endif

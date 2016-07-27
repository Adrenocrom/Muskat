#ifndef ABSTRACTRENDERER
#define ABSTRACTRENDERER

#pragma once


struct SFrameInfo {
	uint id;
	QVector3D pos;
	QVector3D lookAt;
	QVector3D up;
	float	near;
	float far;
	float offangle;
};

struct SFrameBuffer {

};

class AbstractRenderer {
	public:
		virtual void getFrame(SFrameInfo& info, SFrameBuffer& fb) = 0; // Override

		virtual void loadScene(string filename, string suffix) = 0; // Override

		virtual int	getMaxFrames() = 0; // Override

		string getName() {return m_name;}
	protected:
		string m_name;
};

#endif

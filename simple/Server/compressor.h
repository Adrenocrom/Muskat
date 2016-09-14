#ifndef COMPRESSOR_H
#define COMPRESSOR_H

#pragma once

class Config;

struct Edge {
	cv::Point p1;
	cv::Point p2;
};


class Compressor {
public:

	Compressor(Config* config);
	~Compressor();

	QJsonObject compressFrame(FrameInfo& info, FrameBuffer& fb);
private:
	
	// pointer for accessing config
	Config* 	m_config;
	
	// stores quadtree for delaunay triangulation
	QuadTree* 	m_quadtree;

	// compress color image from framebuffer with jpeg or png
	void compressTexture(QJsonObject& jo, FrameBuffer& fb);

	// Mesh compression
	// uses sub methods via config
	void compressMesh(QJsonObject& jo, FrameBuffer& fb);

	// compression Methods
	void compressMesh8Bit(QJsonObject& jo, FrameBuffer& fb);
	void compressMesh16Bit(QJsonObject& jo, FrameBuffer& fb);
	void compressMeshDelaunay(QJsonObject& jo, FrameBuffer& fb);

	vector<cv::Vec6f> delaunay(cv::Mat& img, vector<cv::Point2f>& seeds);

	bool testAngle(cv::Mat& img, Edge& e, int T_angle);
	bool testJoinable(cv::Mat& img, Edge& e, int T_join);

	uint getIndex(vector<cv::Point>& vb, cv::Point p);
};

#endif

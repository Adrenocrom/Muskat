#ifndef COMPRESSOR_H
#define COMPRESSOR_H

#pragma once

class Config;

struct Edge {
	cv::Point p1;
	cv::Point p2;
};

struct QTNode {
	bool isLeaf;
	uint x;	
	uint y;
	uint w;
	uint h;

	int parent;
	int	l1;
	int l2;
	int l3;
	int l4;

	ushort H_x;
	ushort H_y;
	ushort L_x;
	ushort L_y;
	ushort Cx;
	ushort Cy;
};

class QuadTree {
public:
	vector<QTNode> 		 nodes;
	vector<vector<uint>> hnodes;
	vector<uint>	  	 leafs;
	vector<cv::Point2f>	 seeds;

	QuadTree(uint w, uint h, uint max_depth);
	
	void calcCxy(cv::Mat& Gx, cv::Mat& Gy, int T_internal, int T_leaf);
private:

	void createChildren(uint pid, uint current_depth, uint max_depth);
	void calcCxyLeafs(cv::Mat& Gx, cv::Mat& Gy, int& T_leaf);
};

class Compressor {
public:

	Compressor(Config* config);

	QJsonObject compressFrame(FrameInfo& info, FrameBuffer& fb);
private:

	Config* m_config;

	void compressTexture(QJsonObject& jo, FrameBuffer& fb);
	void compressMesh(QJsonObject& jo, FrameBuffer& fb);

	void compressMesh8Bit(QJsonObject& jo, FrameBuffer& fb);
	void compressMesh16Bit(QJsonObject& jo, FrameBuffer& fb);
	void compressMeshDelaunay(QJsonObject& jo, FrameBuffer& fb);

	QuadTree generateQuadTree(FrameBuffer& fb);

	vector<cv::Vec6f> delaunay(cv::Mat& img, vector<cv::Point2f>& seeds, int T_angle, int T_join);

	bool testAngle(cv::Mat& img, Edge& e, int T_angle);
	bool testJoinable(cv::Mat& img, Edge& e, int T_join);
};

#endif

#ifndef COMPRESSOR_H
#define COMPRESSOR_H

#pragma once

class Config;

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
	vector<QTNode> 	nodes;
	vector<uint>	leafs;

	QuadTree(uint w, uint h, uint max_depth);
	
	void calcCxy(uint pid);
	//ushort getH_x(uint pid);
private:

	void createChildren(uint pid, uint current_depth, uint max_depth);
	void calcCxyLeafs(cv::Mat& Gx, cv::Mat& Gy);
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
};

#endif

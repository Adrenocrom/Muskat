#ifndef COMPRESSOR_H
#define COMPRESSOR_H

#pragma once

class Config;

struct Point {
	int x;
	int y;

	ushort z;

	Point(int _x = 0, int _y = 0) : x(_x), y(_y) {}
	Point(const cv::Point& p) : x(p.x), y(p.y) {}

	cv::Point toCVPoint() {
		return cv::Point(x, y);
	}

	bool operator == (const Point& p) {
		if(x == p.x && y == p.y) 
			return true; 
		return false;
	}

	Point& operator = (const Point& p) {x = p.x; y = p.y; return *this;}
	Point& operator = (const cv::Point& p) {x = p.x; y = p.y; return *this;}
};

inline bool operator < (const Point& a, const Point& b) {
	if(a.y < b.y) {
		return true;
	}
	else if(a.y == b.y) {
		if(a.x < b.x) {
			return true;
		}
	}
	return false;
}

inline double norm(const Point& a, const Point& b) {
	double x = (double)(a.x - b.x);
	double y = (double)(a.y - b.y);
	return sqrt(x*x + y*y);
}

struct Edge {
	Point a;
	Point b;

	Edge() {}
	Edge(const Point& _a, const Point& _b) : a(_a), b(_b) {}

	Point getPM() {
		return Point((int)((double)(a.x + b.x) / 2.0), (int)((double)(a.y + b.y) / 2.0));
	}
};

class Compressor {
public:

	Compressor(Config* config);
	~Compressor();

	QJsonObject compressFrame(FrameInfo& info, FrameBuffer& fb);	
	
	// resize quadtree
	void resizeQuadtree();

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
	void compressMesh8Bit(QJsonObject& jo, cv::Mat& img);
	void compressMesh16Bit(QJsonObject& jo, cv::Mat& img);
	void compressMeshDelaunay(QJsonObject& jo, cv::Mat& img);


	vector<cv::Vec6f> delaunay(cv::Mat& img, vector<cv::Point2f>& seeds);

	bool isValid(cv::Mat& img, const Point& p1, const Point& p2);

//	bool testAngle(cv::Mat& img, Edge& e, int T_angle);
//	bool testJoinable(cv::Mat& img, Edge& e, int T_join);

	int getIndex(vector<cv::Point>& vb, cv::Point p);
	int getIndex(map<Point, int>& vertices, const Point& p);
};

#endif

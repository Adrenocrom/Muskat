/***********************************************************
 *
 *
 *						COMPRESSOR HEADER
 *					 =======================
 *
 *		AUTHOR: Josef Schulz
 *
 *		In this file, all parts for the compression
 *		and for the mesh construction parts are declared.
 *		Needed OpenCV image containers and use 
 *		the delaunay triangulation method of opencv.
 *		From Qt, QJsonObject and QJsonArray are used.
 *
 ***********************************************************/

#ifndef COMPRESSOR_H
#define COMPRESSOR_H

#pragma once

#define USE_QUADTREE 		1
#define USE_FLOYDSTEINBERG 	2 //Floyd-Steinberg

class Config;

// point class to map the cv::point2f, needed for the
// reconstruction step
struct Point {
	// image coordinates
	int x;
	int y;

	// 16 bit depth value
	ushort z;

	// texture coordinates
	double u;
	double v;
	double dz; // depth in [0,1]

	Point(int _x = 0, int _y = 0, ushort _z = 0,
		  double _u = 0.0, double _v = 0.0, double _dz = 0.0) : 	x(_x), y(_y), z(_z),
		  															u(_u), v(_v), dz(_dz) {}
	Point(const cv::Point& p) : x(p.x), y(p.y), z(0), u(0.0), v(0.0), dz(0.0) {}

	cv::Point toCVPoint() const {
		return cv::Point(x, y);
	}

	// set z value from image, and calc texture coordinates and calcs dz
	void setP(const cv::Mat& img, int w, int h) {
		z = img.at<ushort>(y, x);
		
		u = (double)x / (double) (w-1);
		v = (double)y / (double) (h-1);
		dz = (double)z / ( ((double) USHRT_MAX) + 1.0) ; // +1 is used to prevent clipping errors
	}

	bool operator == (const Point& p) {
		if(x == p.x && y == p.y) 
			return true; 
		return false;
	}

	void print() const {
		cout<<x<<" "<<y<<endl;
	}

	//Point& operator = (const Point& p) {x = p.x; y = p.y; return *this;}
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
	double x = a.u - b.u;
	double y = a.v - b.v;
	return sqrt(x*x + y*y);
}

inline double length(const Point& a, const Point& b) {
	double x = a.x - b.x;
	double y = a.y - b.y;
	return sqrt(x*x + y*y);
}

inline double squareLength(const Point& a, const Point& b) {
	double x = a.x - b.x;
	double y = a.y - b.y;
	return x*x + y*y;
}

inline Point operator * (const Point& p, const double d) {return Point((int)((double)p.x * d), (int)((double)p.y * d));}
inline Point operator + (const Point& a, const Point& b) {return Point(a.x + b.x, a.y + b.y);}
inline Point operator - (const Point& a, const Point& b) {return Point(a.x - b.x, a.y - b.y);}

// represents an edge with two points
struct Edge {
	Point a;
	Point b;

	Edge() {}
	Edge(const Point& _a, const Point& _b) : a(_a), b(_b) {}

	Point getPM() const {
		return Point((int)((double)(a.x + b.x) / 2.0), (int)((double)(a.y + b.y) / 2.0));
	}

	// calculates the middle point between a and b, additional texture coordinates
	// and z value will be calculated like in the point data structure above
	Point getPM(const cv::Mat& img, int w, int h) const {
		Point p((int)((double)(a.x + b.x) / 2.0), (int)((double)(a.y + b.y) / 2.0));
		p.z = img.at<ushort>(p.y, p.x);
		
		p.u = (double)p.x / (double) (w-1);
		p.v = (double)p.y / (double) (h-1);
		p.dz = (double)p.z / (((double) USHRT_MAX) + 1.0);
		return p;
	}
};

// calc the depth value linear between a and b of e,
// for the comparison with real value
inline double calcPMDepth(const Edge& e) {
	return (e.a.dz + e.b.dz) / 2.0;
}

// same like calcPMDepth but return an int value created
// from the ushort z values
inline int calcIntPMDepth(const Edge& e) {
	return ((int)(e.a.z + e.b.z)) / 2;
}


class Compressor {
public:

	Compressor(Config* config);
	~Compressor();

	QJsonObject compressFrame(FrameInfo& info, FrameBuffer& fb);	
	
	cv::Mat*	getDelaunayImage();
	cv::Mat*	getMeshImage();
	cv::Mat*	getSeedImage();
	cv::Mat*	getSobelXImage();
	cv::Mat*	getSobelYImage();
	cv::Mat*	getFeatureImage();
	clock_t		getCTimeTexture();
	clock_t		getCTimeFull();
	clock_t		getCTimeSeeds();
	clock_t		getCTimeDelaunay();
	clock_t		getCTimeTranform();

	// resize quadtree, it is realy slow,
	// but differnt quadtrees with respect to the maximum depth could
	// be precreated
	void resizeQuadtree();

private:
	
	// pointer for accessing config
	Config* 	m_config;

	// predefined colors
	cv::Scalar 	m_color_point;
	cv::Scalar 	m_color_edge;
	cv::Scalar 	m_color_green;
	cv::Scalar 	m_color_red;
	cv::Scalar 	m_color_blue;
	cv::Scalar 	m_color_black;
	cv::Scalar 	m_color_triangle;

	// some images, for drawing
	cv::Mat		m_delaunay_image;
	cv::Mat		m_mesh_image;
	cv::Mat		m_seed_image;
	cv::Mat		m_sobel_x_image;
	cv::Mat		m_sobel_y_image;
	cv::Mat		m_feature_image;
	clock_t		m_ctime_texture;
	clock_t		m_ctime_full;
	clock_t		m_ctime_seeds;
	clock_t		m_ctime_delaunay;
	clock_t		m_ctime_transform;

	// stores quadtree for delaunay triangulation
	QuadTree* 	m_quadtree;

	// compress color image from framebuffer with jpeg or png
	void compressTexture(QJsonObject& jo, FrameBuffer& fb);

	// Mesh compression
	// uses sub methods via config
	void compressMesh(QJsonObject& jo, FrameBuffer& fb);

	// compression Methods (full connected meshes)
	void compressMesh8Bit(QJsonObject& jo, cv::Mat& img);
	void compressMesh16Bit(QJsonObject& jo, cv::Mat& img);

	// create seeds with quadtree or with Floyd-Steinberg
	void compressMeshDelaunay(QJsonObject& jo, cv::Mat& img);

	// performs delaunay triangulation, input are the depth image and seed points,
	// the result is list of triangles in an 2D space
	vector<cv::Vec6f> delaunay(cv::Mat& img, list<cv::Point2f>& seeds);

	// returns a list of seedpoints, produces with floyd steinberg
	list<cv::Point2f> floydSteinberg(cv::Mat& gx, cv::Mat& gy, double T = 0.5, double gamma = 0.5);

	// check if a edge is valid
	bool isValid(cv::Mat& img, const Edge& e);

	// check if a edge is joinable
	bool isJoinable(cv::Mat& img, const Edge& e);

	// check if vertices of a triangle are collinear 
	bool isCollinear(const Point& p1, const Point& p2, const Point& p3);

	// splitts a tringle, performs the refinement step
	// by checking for valid edges and split with the right case
	vector<Point> splitTriangle(cv::Mat& img, const Point& a, const Point& b, const Point& c);

	// does the same like splitTriangles but draws the refinement
	// of the triangle
	vector<Point> splitTriangleDraw(cv::Mat& img, const Point& a, const Point& b, const Point& c, cv::Mat* out = nullptr);

	// this method calculates the nears and best points from p to a
	Point getMaxJoinable(cv::Mat& img, const Point& p, const Point& a);
	
	// creates a list of all possible joinable possitions from p to a
	vector<Point> getMaxJoinablePoints(cv::Mat& img, const Point& p, const Point& a);
	
	// finds the best joinable points (from a to c) and (from b to c) while both points
	// has to be joinable two
	pair<Point, Point> getMaxJoinables(cv::Mat& img, const Point& a, const Point& b, const Point& c);

	// calc index from vertices
	int getIndex(vector<cv::Point>& vb, cv::Point p);			// slow
	int getIndex(map<Point, int>& vertices, const Point& p);	// faster
};

#endif

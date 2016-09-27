#include "muskat.h"

Compressor::Compressor(Config* config) {
	m_config = config;

	// generate quadtree, needs to be recreated after resize
	m_quadtree = nullptr;
	m_quadtree = new QuadTree(	m_config->getMeshWidth(),
								m_config->getMeshHeight(),
								m_config->getMaxDepth());
}

Compressor::~Compressor() {
	SAFE_DELETE(m_quadtree);
}

QJsonObject Compressor::compressFrame(FrameInfo& info, FrameBuffer& fb) {
	QJsonObject jo;
	
	compressTexture(jo, fb);
	compressMesh(jo, fb);

	return jo;
}

void Compressor::compressTexture(QJsonObject& jo, FrameBuffer& fb) {
	std::vector<uchar> rgb;	//buffer for coding
	std::vector<int> param(2);

	if(m_config->getTextureCompressionMethod() == "jpeg") {
		param[0] = cv::IMWRITE_JPEG_QUALITY;
		param[1] = m_config->getTextureCompressionQuality();	//default(95) 0-100
		cv::imencode(".jpg", fb.rgb, rgb, param);
	}
	else if(m_config->getTextureCompressionMethod() == "png") {
		param[0] = cv::IMWRITE_PNG_COMPRESSION;
		param[1] = m_config->getTextureCompressionQuality();	//default(3) 0-9
		cv::imencode(".png", fb.rgb, rgb, param);
	}
		
	QByteArray ba_rgb;
	ba_rgb.append((const char*)rgb.data(), rgb.size());

	jo["rgb"] = QString(ba_rgb.toBase64());
}

void Compressor::compressMesh(QJsonObject& jo, FrameBuffer& fb) {
	cout<<"begin compression"<<endl;
	cv::Mat img;

	if(m_config->hasDifferentSize()) 
		cv::resize(fb.depth, img, cv::Size(m_config->getMeshWidth(), m_config->getMeshHeight()));
	else
		img = fb.depth;

	if(m_config->getMeshMode() == "full") {
		if(m_config->getMeshPercesion() == "8bit") {
			compressMesh8Bit(jo, img);
		} else {
			compressMesh16Bit(jo, img);
		}
	} else {
		compressMeshDelaunay(jo, img);
	
	}
	cout<<"end compression"<<endl;
}

void Compressor::compressMesh8Bit(QJsonObject& jo, cv::Mat& img) {
	std::vector<uchar> 	depth;
	std::vector<int> 	param(2);
	
	param[0] = cv::IMWRITE_PNG_COMPRESSION;
	param[1] = m_config->getMeshCompression();//default(3) 0-9
	cv::imencode(".png", img, depth, param);
	
	QByteArray ba_depth;
	ba_depth.append((const char*)depth.data(), depth.size());

	jo["depth"] = QString(ba_depth.toBase64());
}

void Compressor::compressMesh16Bit(QJsonObject& jo, cv::Mat& img) {
	std::vector<uchar> 	depth;
	std::vector<int> 	param(2);

	cv::Mat rgbDepth(img.rows, img.cols, CV_8UC3);
	ushort* ptr = reinterpret_cast<ushort*>(img.data);
	for(int y = 0; y < img.cols; ++y) {
		for(int x = 0; x < img.rows; ++x) {
			uchar *p = (uchar*)ptr;

			rgbDepth.at<cv::Vec3b>(y,x)[0] = 0;		// B
			rgbDepth.at<cv::Vec3b>(y,x)[1] = p[0];	// G
			rgbDepth.at<cv::Vec3b>(y,x)[2] = p[1];	// R

			ptr++;
		}
	}
	
	param[0] = cv::IMWRITE_PNG_COMPRESSION;
	param[1] = m_config->getMeshCompression();	//default(3) 0-9
	cv::imencode(".png", rgbDepth, depth, param);
	
	QByteArray ba_depth;
	ba_depth.append((const char*)depth.data(), depth.size());

	jo["depth"] = QString(ba_depth.toBase64());
}

void Compressor::compressMeshDelaunay(QJsonObject& jo, cv::Mat& img) {
	cv::Mat grad_x; 
	cv::Mat grad_y;

	// calc sobel gradients for 2 dimensions and store them
	Sobel( img, grad_x, -1, 1, 0, 3, 1, 0, cv::BORDER_DEFAULT );
	Sobel( img, grad_y, -1, 0, 1, 3, 1, 0, cv::BORDER_DEFAULT );

	m_quadtree->setTleaf(m_config->getTleaf());
	m_quadtree->setTinternal(m_config->getTinternal());

	// calc seeds from quadtree
	vector<cv::Point2f>	seeds = m_quadtree->generateSeeds(&grad_x, &grad_y);

	vector<cv::Vec6f> triangles = delaunay(img, seeds);

	// TODO refine
	
	vector<cv::Point> pt(3);
	vector<Point> points(3);
	map<Point, int>	map_vertices;
	
	cv::Size size = img.size();
	cv::Rect rect(0,0, size.width, size.height);

	cv::Mat meshImage(m_config->getMeshWidth(), m_config->getMeshHeight(), CV_8UC3, cv::Scalar(255,255,255));

	cv::Scalar 	color_point(100, 50, 50);
	cv::Scalar 	color_edge(100,100,100);
	cv::Scalar 	color_green(0,150,0);
	cv::Scalar 	color_red(0,0,150);
	cv::Scalar 	color_triangle(50,100,50);

	QJsonArray ja_indices;
	QJsonArray ja_vertices;

	bool first_index = true;
	ostringstream osstream_i;
	osstream_i<<"[";
	uint t_size = triangles.size();
	for(uint i = 0; i < t_size; ++i ) {
		cv::Vec6f t = triangles[i];
        pt[0] = cv::Point(cvRound(t[0]), cvRound(t[1]));
        pt[1] = cv::Point(cvRound(t[2]), cvRound(t[3]));
        pt[2] = cv::Point(cvRound(t[4]), cvRound(t[5]));
		
		if ( rect.contains(pt[0]) && rect.contains(pt[1]) && rect.contains(pt[2]) ) {
			points[0] = pt[0];
			points[1] = pt[1];
			points[2] = pt[2];
			points[0].z = img.at<ushort>(points[0].y, points[0].x);
			points[1].z = img.at<ushort>(points[1].y, points[1].x);
			points[2].z = img.at<ushort>(points[2].y, points[2].x);

			if(!m_config->getUseBackground() && points[0].z == USHRT_MAX && points[1].z == USHRT_MAX && points[2].z == USHRT_MAX) {
				continue;
			}

			vector<Edge> invalid_edges;
			if(isValid(img, points[0], points[1])) {
				line(meshImage, pt[0], pt[1], color_edge, 1, CV_AA, 0);
			} else {
				line(meshImage, pt[0], pt[1], color_red, 1, CV_AA, 0);
				invalid_edges.push_back(Edge(points[0], points[1]));
			}
			if(isValid(img, points[1], points[2])) {
           		line(meshImage, pt[1], pt[2], color_edge, 1, CV_AA, 0);
			} else {
           		line(meshImage, pt[1], pt[2], color_red, 1, CV_AA, 0);
				invalid_edges.push_back(Edge(points[1], points[2]));
			}
			if(isValid(img, points[2], points[0])) {
            	line(meshImage, pt[2], pt[0], color_edge, 1, CV_AA, 0);
			} else {
            	line(meshImage, pt[2], pt[0], color_red, 1, CV_AA, 0);
				invalid_edges.push_back(Edge(points[2], points[0]));
			}

			if(invalid_edges.size() > 1) {
			
			}

			// Draw delaunay triangles
			const cv::Point* ppt[1] = { pt.data() };

			int npt[] = { 3 };
			fillPoly(meshImage, ppt, npt, 1, color_triangle);
/*

 			circle( meshImage, pt[0], 2, color_point, CV_FILLED, CV_AA, 0 );
 			circle( meshImage, pt[1], 2, color_point, CV_FILLED, CV_AA, 0 );
 			circle( meshImage, pt[2], 2, color_point, CV_FILLED, CV_AA, 0 );*/
/*
			ja_indices.push_back(getIndex(map_vertices, points[0]));
			ja_indices.push_back(getIndex(map_vertices, points[1]));
			ja_indices.push_back(getIndex(map_vertices, points[2]));
*/
			if(first_index) {
				osstream_i<<getIndex(map_vertices, points[0])<<","<<getIndex(map_vertices, points[1])<<","<<getIndex(map_vertices, points[2]);
				first_index = false;
			}
			else {	   		
				osstream_i<<","<<getIndex(map_vertices, points[0])<<","<<getIndex(map_vertices, points[1])<<","<<getIndex(map_vertices, points[2]);
			}
		}
	}

	osstream_i<<"]";

	// create vector
	int num_texCoord = 0;
	vector<Point> vertices(map_vertices.size());
	for(auto it = map_vertices.begin(); it != map_vertices.end(); ++it) {
		vertices[it->second] = it->first;
		num_texCoord += 2;
	}

	double invWidth 	= 1.0 / (double)m_config->getMeshWidth();
	double invHeight 	= 1.0 / (double)m_config->getMeshHeight();
	double invMax		= 1.0 / ((double)(USHRT_MAX) + 1.0);
	
	if(vertices.size() > 0) {
		ostringstream osstream_v;
		Point& p = vertices[0];
		osstream_v<<"["<<((double)p.x * invWidth)<<","<<((double)p.y * invHeight)<<","<<((double)img.at<ushort>(p.y, p.x) * invMax);
	
		int v_size = vertices.size();
		for(int i = 1; i < v_size; ++i) {
			p = vertices[i];
			osstream_v<<","<<((double)p.x * invWidth)<<","<<((double)p.y * invHeight)<<","<<((double)img.at<ushort>(p.y, p.x) * invMax);
		
			//ja_vertices.push_back((double)p.x * invWidth);
			//ja_vertices.push_back((double)p.y * invHeight);
			//ja_vertices.push_back((double)(img.at<ushort>(p.x, p.y)) * invMax);
		}
	
		osstream_v<<"]";
    
		imwrite("res/delaunay.png", meshImage);

		jo["indices"] 	= QString(osstream_i.str().c_str());//ja_indices;
		jo["vertices"] 	= QString(osstream_v.str().c_str());//ja_vertices;
	}
	jo["numTexCoord"] = num_texCoord;
}

int Compressor::getIndex(vector<cv::Point>& vertices, cv::Point p) {
	int size = vertices.size();
	for(int i = 0; i < size; ++i) {
		if(vertices[i].x == p.x && vertices[i].y == p.y)
			return i;
	}

	vertices.push_back(p);
	return size;
}

int Compressor::getIndex(map<Point, int>& vertices, const Point& p) {
	int size = vertices.size();
	auto ret  = vertices.insert(pair<Point, int>(p, size));
	return ret.first->second;
}

vector<cv::Vec6f> Compressor::delaunay(cv::Mat& img, vector<cv::Point2f>& seeds) {
	cv::Rect rect(0, 0, img.rows, img.cols);
	cv::Subdiv2D subdiv(rect);

	uint size = seeds.size();
	for(uint i = 0; i < size; ++i) {
		subdiv.insert(seeds[i]);
	}

	vector<cv::Vec6f> triangleList;
    subdiv.getTriangleList(triangleList);

	return triangleList;
}

void Compressor::resizeQuadtree() {
	cout<<"resize begin: "<<m_config->getMaxDepth()<<endl;
	// resize Quadtree
	SAFE_DELETE(m_quadtree);
	m_quadtree = new QuadTree( 	m_config->getMeshWidth(), 
								m_config->getMeshHeight(),
								m_config->getMaxDepth());
	cout<<"resize end"<<endl;
}

bool Compressor::isValid(cv::Mat& img, const Point& p1, const Point& p2) {
	double value_a = fabs((double)p1.z - (double)p2.z);
	double value_b = norm(p1, p2) * ((double)p1.z + (double)p2.z);
	return (value_a / value_b) < m_config->getTangle();
}
/*
bool Compressor::testJoinable(cv::Mat& img, Edge& e, int T_join) {
	cv::Point pm = cv::Point(cvRound((e.p1.x + e.p2.x) / 2), cvRound((e.p1.y + e.p2.y) / 2));

	double a = (img.at<ushort>(e.p2.y, e.p2.x) - img.at<ushort>(pm.y, pm.x)); 
	double b = (img.at<ushort>(pm.y, pm.x) - img.at<ushort>(e.p1.y, e.p1.x)); 

	return abs(a - b) < T_join;
}*/

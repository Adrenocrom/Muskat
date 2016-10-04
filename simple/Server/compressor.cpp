#include "muskat.h"

Compressor::Compressor(Config* config) {
	m_config = config;

	// generate quadtree, needs to be recreated after resize
	m_quadtree = nullptr;
	m_quadtree = new QuadTree(	m_config->getMeshWidth(),
								m_config->getMeshHeight(),
								m_config->getMaxDepth());

	m_color_point		= cv::Scalar(120, 80, 80);
	m_color_edge		= cv::Scalar(120, 120, 120);
	m_color_green		= cv::Scalar(0, 120, 0);
	m_color_red			= cv::Scalar(0, 0, 120);
	m_color_triangle	= cv::Scalar(0, 120, 0);
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
	cv::Mat res;
	cv::Mat img;

	if(m_config->hasDifferentSize()) {
		cv::resize(fb.depth, res, cv::Size(m_config->getMeshWidth(), m_config->getMeshHeight()));
		
		if(m_config->getSmoothDepth())
			cv::medianBlur ( res, img, 3 );
		else img = res;
	}
	else img = fb.depth;


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

	double invMax		= 1.0 / ((double)(USHRT_MAX) + 1.0);
	double invWidth 	= 1.0 / (double)(m_config->getMeshWidth() - 1);
	double invHeight 	= 1.0 / (double)(m_config->getMeshHeight()- 1);

	// calc sobel gradients for 2 dimensions and store them
	Sobel( img, grad_x, -1, 1, 0, 3, 1, 0, cv::BORDER_DEFAULT );
	Sobel( img, grad_y, -1, 0, 1, 3, 1, 0, cv::BORDER_DEFAULT );

	m_quadtree->setTleaf(m_config->getTleaf());
	m_quadtree->setTinternal(m_config->getTinternal());

	// calc seeds from quadtree
	list<cv::Point2f>	seeds = m_quadtree->generateSeeds(&grad_x, &grad_y);

	if(m_config->preBackgroundSubtraction()) {
		seeds.remove_if([&](const cv::Point2f& p) {
			if(img.at<ushort>((int)p.y, (int)p.x) == USHRT_MAX)
				return true;
			return false;
		});
	}

	vector<cv::Vec6f> triangles = delaunay(img, seeds);

	
	vector<cv::Point> pt(3);
	vector<Point> points(3);
	map<Point, int>	map_vertices;
	
	cv::Size size = img.size();
	cv::Rect rect(0,0, size.width, size.height);

	cv::Mat meshImage(m_config->getMeshWidth(), m_config->getMeshHeight(), CV_8UC3, cv::Scalar(255,255,255));

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


			if(m_config->praBackgroundSubtraction() && (points[0].z == USHRT_MAX || points[1].z == USHRT_MAX || points[2].z == USHRT_MAX)) {
				continue;
			}

			// Draw delaunay triangles
			const cv::Point* ppt[1] = { pt.data() };
			int npt[] = { 3 };
			fillPoly(meshImage, ppt, npt, 1, m_color_triangle);
/*

 			circle( meshImage, pt[0], 2, m_color_point, CV_FILLED, CV_AA, 0 );
 			circle( meshImage, pt[1], 2, m_color_point, CV_FILLED, CV_AA, 0 );
 			circle( meshImage, pt[2], 2, m_color_point, CV_FILLED, CV_AA, 0 );*/

	
			points[0].u  = (double) points[0].x * invWidth;
			points[0].v  = (double) points[0].y * invHeight;
			points[0].dz = (double) points[0].z * invMax;

			points[1].u  = (double) points[1].x * invWidth;
			points[1].v  = (double) points[1].y * invHeight;
			points[1].dz = (double) points[1].z * invMax;

			points[2].u  = (double) points[2].x * invWidth;
			points[2].v  = (double) points[2].y * invHeight;
			points[2].dz = (double) points[2].z * invMax;

			vector<Point> n_points = splitTriangle(img, points[0], points[1], points[2], &meshImage);
			uint n_size = n_points.size();
			for(uint i = 0; i < n_size; ++i) {
				if(first_index) {
					osstream_i<<getIndex(map_vertices, n_points[i]);
					first_index = false;
				} else {
					osstream_i<<","<<getIndex(map_vertices, n_points[i]);
				}
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

	int v_size = vertices.size();
	if(v_size > 0) {
		ostringstream osstream_v;
		Point& p = vertices[0];
		osstream_v<<"["<<p.u<<","<<p.v<<","<<p.dz;
		for(int i = 1; i < v_size; ++i) {
			p = vertices[i];
			osstream_v<<","<<p.u<<","<<p.v<<","<<p.dz;
		}
	
		osstream_v<<"]";
    
		imwrite("res/delaunay.png", meshImage);

		jo["indices"] 	= QString(osstream_i.str().c_str());
		jo["vertices"] 	= QString(osstream_v.str().c_str());
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

vector<cv::Vec6f> Compressor::delaunay(cv::Mat& img, list<cv::Point2f>& seeds) {
	cv::Rect rect(0, 0, img.rows, img.cols);
	cv::Subdiv2D subdiv(rect);

	auto end = seeds.end();
	for(auto it = seeds.begin(); it != end; it++) {
		subdiv.insert(*it);
	}

	vector<cv::Vec6f> triangleList;
    subdiv.getTriangleList(triangleList);

	return triangleList;
}

void Compressor::resizeQuadtree() {
	// resize Quadtree
	SAFE_DELETE(m_quadtree);
	m_quadtree = new QuadTree( 	m_config->getMeshWidth(), 
								m_config->getMeshHeight(),
								m_config->getMaxDepth());
}

bool Compressor::isValid(const Edge& e) {
	double value_a = fabs(e.a.dz - e.b.dz);
	double value_b = norm(e.a, e.b) * (e.a.dz + e.b.dz);
	if(value_b == 0.0) return true;
	return (value_a / value_b) < m_config->getTangle();
}

bool Compressor::isJoinable(cv::Mat& img, const Edge& e) {
	Point pm = e.getPM(img, m_config->getMeshWidth(), m_config->getMeshHeight());
	double a = e.b.dz - pm.dz;
	double b = pm.dz  - e.a.dz;
	return fabs(a - b) < m_config->getTjoin();
}

vector<Point> Compressor::splitTriangle(cv::Mat& img, const Point& a, const Point& b, const Point& c, cv::Mat* out) {
	vector<Point> triangles;

	Edge ab(a, b);
	Edge bc(b, c);
	Edge ca(c, a);

	vector<Edge*> invalid_edges;
	int			  valid_edge = -1;

	if(out == nullptr) {
		if(isValid(ab)) valid_edge = 0;
		else invalid_edges.push_back(&ab);
		if(isValid(bc)) valid_edge = 1;
		else invalid_edges.push_back(&bc);
		if(isValid(ca)) valid_edge = 2;
		else invalid_edges.push_back(&ca);
	}
	else {
		if(isValid(ab)) {
			valid_edge = 0;
			line(*out, a.toCVPoint(), b.toCVPoint(), m_color_edge, 1, CV_AA, 0);
		} else {
			invalid_edges.push_back(&ab);
			line(*out, a.toCVPoint(), b.toCVPoint(), m_color_red, 1, CV_AA, 0);
		}
		if(isValid(bc)) {
			valid_edge = 1;
			line(*out, b.toCVPoint(), c.toCVPoint(), m_color_edge, 1, CV_AA, 0);
		} else {
			invalid_edges.push_back(&bc);
			line(*out, b.toCVPoint(), c.toCVPoint(), m_color_red, 1, CV_AA, 0);
		}
		if(isValid(ca)) {
			valid_edge = 2;
			line(*out, c.toCVPoint(), a.toCVPoint(), m_color_edge, 1, CV_AA, 0);
		} else {
			invalid_edges.push_back(&ca);
			line(*out, c.toCVPoint(), a.toCVPoint(), m_color_red, 1, CV_AA, 0);
		}
	}

	int i_size = invalid_edges.size();
	if(i_size == 3) {
		triangles.resize(9);
		triangles[0] = a;
		triangles[1] = getMaxJoinable(img, a, b);
		triangles[2] = getMaxJoinable(img, a, c);

		triangles[3] = b;
		triangles[4] = getMaxJoinable(img, b, c);
		triangles[5] = getMaxJoinable(img, b, c);

		triangles[6] = c;
		triangles[7] = getMaxJoinable(img, c, a);
		triangles[8] = getMaxJoinable(img, c, b);
	}
	
	if(i_size == 2) {
		triangles.resize(9);
		if(valid_edge == 0) {
			triangles[0] = c;
			triangles[1] = getMaxJoinable(img, c, a);
			triangles[2] = getMaxJoinable(img, c, b);

			Point temp = getMaxJoinable(img, b, c);
			triangles[3] = a;
			triangles[4] = temp;
			triangles[5] = getMaxJoinable(img, a, c);

			triangles[6] = b;
			triangles[7] = temp;
			triangles[8] = a;
		}
		else if(valid_edge == 1) {
			triangles[0] = a;
			triangles[1] = getMaxJoinable(img, a, b);
			triangles[2] = getMaxJoinable(img, a, c);

			Point temp = getMaxJoinable(img, c, a);
			triangles[3] = b;
			triangles[4] = temp;
			triangles[5] = getMaxJoinable(img, b, a);

			triangles[6] = c;
			triangles[7] = temp;
			triangles[8] = b;
		}
		else if(valid_edge == 2) {
			triangles[0] = b;
			triangles[1] = getMaxJoinable(img, b, c);
			triangles[2] = getMaxJoinable(img, b, a);

			Point temp = getMaxJoinable(img, a, b);
			triangles[3] = c;
			triangles[4] = temp;
			triangles[5] = getMaxJoinable(img, c, a);

			triangles[6] = a;
			triangles[7] = temp;
			triangles[8] = c;
		}
	} else {
		triangles.resize(3);
		triangles[0] = a;
		triangles[1] = b;
		triangles[2] = c;
	}

	return triangles;
}

Point Compressor::getMaxJoinable(cv::Mat& img, const Point& p, const Point& a) {
	Point m = p;
	Point n;
	Point d = a - p;
	
	for(double i = 0.0; i <= 1.0; i += 0.2) {
		n = p + d * i;
		n.setP(img, m_config->getMeshWidth(), m_config->getMeshHeight());

		if(isJoinable(img, Edge(p, n))) {
			m = n;
		} else {
			return m;
		}
	}

	return m;
}

/***********************************************************
 *
 *
 *						COMPRESSOR SOURCE
 *					 =======================
 *
 *		AUTHOR: Josef Schulz
 *
 *		Definition of all declared compression
 *		and mesh creating methods.
 *		More information in compressor.h
 *
 ***********************************************************/

#include "muskat.h"

// set default condig and define colors
// and clear drawable images
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
	m_color_blue		= cv::Scalar(120, 0, 0);
	m_color_black		= cv::Scalar(0, 0, 0);
	m_color_triangle	= cv::Scalar(0, 120, 0);
	
	m_delaunay_image = cv::Mat(m_config->getMeshWidth()*4, m_config->getMeshHeight()*4, CV_8UC3, cv::Scalar(255,255,255));
	m_feature_image  = cv::Mat(m_config->getMeshWidth(), m_config->getMeshHeight(), CV_64FC1, 0.0);
	m_mesh_image  	 = cv::Mat(m_config->getMeshWidth()*4, m_config->getMeshHeight()*4, CV_8UC3, cv::Scalar(255,255,255));
	m_seed_image 	 = cv::Mat(m_config->getMeshWidth(), m_config->getMeshHeight(), CV_8UC4, cv::Scalar(255,255,255,0));
	
	m_ctime_texture 	= 0;
	m_ctime_full		= 0;
	m_ctime_seeds		= 0;
	m_ctime_delaunay	= 0;
	m_ctime_transform	= 0;
}

// delete quadtree
Compressor::~Compressor() {
	SAFE_DELETE(m_quadtree);
}

// wrapper method seperates the creation of texture
// and mesh, could handled parallel
QJsonObject Compressor::compressFrame(FrameInfo& info, FrameBuffer& fb) {
	QJsonObject jo;

	// set all times to zero, not all will filled at a call,
	// but all used are greater than zero for shure
	m_ctime_texture 	= 0;
	m_ctime_full		= 0;
	m_ctime_seeds		= 0;
	m_ctime_delaunay	= 0;
	m_ctime_transform	= 0;
	

	clock_t c_begin = clock();

	// compress texture with png or jpeg
	compressTexture(jo, fb);
	
	clock_t c_end	= clock();
	m_ctime_texture = ((c_end - c_begin) * 1000.0) / CLOCKS_PER_SEC;
	
	// compress mesh, times will messured for the steps sperate
	compressMesh(jo, fb);
	
	
	return jo;
}

// compress texture with jpeg or png and create an base64 string
// and insert them into the json object 
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
	
	// byte array needed to create base64 string with qt
	QByteArray ba_rgb;
	ba_rgb.append((const char*)rgb.data(), rgb.size());

	// write base64 string to jsonrpc
	jo["rgb"] = QString(ba_rgb.toBase64());
}

// wrapper method for mesh compression, seperates different
// methods
void Compressor::compressMesh(QJsonObject& jo, FrameBuffer& fb) {
	cout<<"begin compression"<<endl;
	cv::Mat res;
	cv::Mat img;

	if(m_config->hasDifferentSize()) {
		cv::resize(fb.depth, res, cv::Size(m_config->getMeshWidth(), m_config->getMeshHeight()), 0, 0, cv::INTER_CUBIC);
		
		if(m_config->getSmoothDepth())
			cv::medianBlur ( res, img, 3 );
		else img = res;
	}
	else img = fb.depth;


	if(m_config->getMeshMode() == "full") {
		if(m_config->getMeshPrecision() == "8bit") {
			clock_t c_begin = clock();
			compressMesh8Bit(jo, img);
			clock_t c_end = clock();
			m_ctime_full = ((c_end - c_begin) * 1000.0) / CLOCKS_PER_SEC;
		} else {
			clock_t c_begin = clock();
			compressMesh16Bit(jo, img);
			clock_t c_end = clock();
			m_ctime_full = ((c_end - c_begin) * 1000.0) / CLOCKS_PER_SEC;
		}
	} else {
		compressMeshDelaunay(jo, img);
	
	}
	cout<<"end compression"<<endl;
}

// compress depth image with png
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

// split 16 bit value in two color channels, to
// make them accessabel in the webgl shader
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

// mesh compression with delaunay based methods
void Compressor::compressMeshDelaunay(QJsonObject& jo, cv::Mat& img) {
	clock_t c_begin = clock();
	// Messure generating seeds

	cv::Mat sx, ax; 
	cv::Mat sy, ay;

	double invMax		= 1.0 / ((double)(USHRT_MAX) + 2.0);
	double invWidth 	= 1.0 / (double)(m_config->getMeshWidth() - 1);
	double invHeight 	= 1.0 / (double)(m_config->getMeshHeight()- 1);

	// calc sobel gradients for 2 dimensions and store them
	Sobel( img, sx, CV_32F, 1, 0, 3, 1, 0, cv::BORDER_DEFAULT ); // need 32 bit float, using ushort
	Sobel( img, sy, CV_32F, 0, 1, 3, 1, 0, cv::BORDER_DEFAULT ); // is not possible, negative values would be lost.
	cv::absdiff(sx, cv::Scalar::all(0), ax); 	// calc abs 
	cv::absdiff(sy, cv::Scalar::all(0), ay);
	ax.convertTo(m_sobel_x_image, CV_16U); 		// and convert back to ushort
	ay.convertTo(m_sobel_y_image, CV_16U);

	// set quadtree thresholds
	m_quadtree->setTleaf(m_config->getTleaf());
	m_quadtree->setTinternal(m_config->getTinternal());

	// calc seeds from quadtree or floadSteinberg
	list<cv::Point2f>	seeds;
	if(m_config->getSeedMode() == "quadtree") seeds = m_quadtree->generateSeeds(img, m_sobel_x_image, m_sobel_y_image, m_config->preBackgroundSubtraction());
	else									  seeds = floydSteinberg(m_sobel_x_image, m_sobel_y_image, m_config->getTthreshold(), m_config->getGamma());

	clock_t c_end = clock();
	m_ctime_seeds = ((c_end - c_begin) * 1000.0) / CLOCKS_PER_SEC;
	cout<<"Seeds: "<<m_ctime_seeds<<endl;
	
	// no time will messure here, now we can create the seedimages
	m_seed_image 	 = cv::Mat(m_config->getMeshWidth(), m_config->getMeshHeight(), CV_8UC4, cv::Scalar(255,255,255,0));
	m_quadtree->drawGenerateSeeds(m_seed_image, img, m_sobel_x_image, m_sobel_y_image, m_config->preBackgroundSubtraction());

	c_begin = clock();
	// messure delaunay
	vector<cv::Vec6f> triangles = delaunay(img, seeds); // create triangles from seeds

	c_end = clock();
	m_ctime_delaunay = ((c_end - c_begin) * 1000.0) / CLOCKS_PER_SEC;
	cout<<"Delaunay: "<<m_ctime_delaunay<<endl;
	
	c_begin = clock();
	vector<cv::Point> pt(3);
	vector<Point> points(3);
	map<Point, int>	map_vertices;
	
	cv::Size size = img.size();
	cv::Rect rect(0,0, size.width, size.height);
	bool first_index = true;

	// this stringstream will be useds to write the vertices
	// and indices to the string,
	// because qjsonarray do not provide a method for resizeing the array first
	// the push back method is incredible slow
	ostringstream osstream_i;
	osstream_i<<"[";

	// now all triagnes will insert in the final mesh
	// refinement will be done in this loop too
	uint t_size = triangles.size();
	for(uint i = 0; i < t_size; ++i ) {
		cv::Vec6f t = triangles[i];
        pt[0] = cv::Point(cvRound(t[0]), cvRound(t[1]));
        pt[1] = cv::Point(cvRound(t[2]), cvRound(t[3]));
        pt[2] = cv::Point(cvRound(t[4]), cvRound(t[5]));
		
		// one of the triangles is bigger than the others, we needed to check to discard this triangle 
		// the big one is needed for the delaunay triangluation, the possition in the vector is arbitrary
		if ( rect.contains(pt[0]) && rect.contains(pt[1]) && rect.contains(pt[2]) ) {
			points[0] = pt[0];		// opencv points will be mapped to the point structure
			points[1] = pt[1];
			points[2] = pt[2];
			points[0].z = img.at<ushort>(points[0].y, points[0].x); // z values are needed
			points[1].z = img.at<ushort>(points[1].y, points[1].x);
			points[2].z = img.at<ushort>(points[2].y, points[2].x);

			// check for triangles, where all vertices are background vertices
			if(m_config->praBackgroundSubtraction() && (points[0].z == USHRT_MAX || points[1].z == USHRT_MAX || points[2].z == USHRT_MAX)) {
				continue;
			}

			// calculate the texture coordinates and the dz value
			points[0].u  = (double) points[0].x * invWidth;
			points[0].v  = (double) points[0].y * invHeight;
			points[0].dz = (double) points[0].z * invMax;

			points[1].u  = (double) points[1].x * invWidth;
			points[1].v  = (double) points[1].y * invHeight;
			points[1].dz = (double) points[1].z * invMax;

			points[2].u  = (double) points[2].x * invWidth;
			points[2].v  = (double) points[2].y * invHeight;
			points[2].dz = (double) points[2].z * invMax;

			// calculate the refinement, if its disabled the insert points are returned
			vector<Point> n_points = splitTriangle(img, points[0], points[1], points[2]);

			// calculate indizes of the vertices and store them into the stringstream
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

	// create vector of the used map-structure
	int num_texCoord = 0;
	vector<Point> vertices(map_vertices.size());
	for(auto it = map_vertices.begin(); it != map_vertices.end(); ++it) {
		vertices[it->second] = it->first;
		num_texCoord += 2;
	}

	// insert all verticies in a stringstream (remember its faster as the qjsonarray)
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
    
		// add both vertices and indices to the json object
		jo["indices"] 	= QString(osstream_i.str().c_str()); 
		jo["vertices"] 	= QString(osstream_v.str().c_str());
	}
	jo["numTexCoord"] = num_texCoord;

	c_end = clock();
	m_ctime_transform = ((c_end - c_begin) * 1000.0) / CLOCKS_PER_SEC;
	cout<<"Transform: "<<m_ctime_transform<<endl;
	
	// Now the refinement step will be calculated again, to draw the infos without messure the time here
	// Draw delaunay
	m_delaunay_image = cv::Mat(m_config->getMeshWidth()*4, m_config->getMeshHeight()*4, CV_8UC3, cv::Scalar(255,255,255));
	m_mesh_image  	 = cv::Mat(m_config->getMeshWidth()*4, m_config->getMeshHeight()*4, CV_8UC3, cv::Scalar(255,255,255));
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

			points[0].u  = (double) points[0].x * invWidth;
			points[0].v  = (double) points[0].y * invHeight;
			points[0].dz = (double) points[0].z * invMax;

			points[1].u  = (double) points[1].x * invWidth;
			points[1].v  = (double) points[1].y * invHeight;
			points[1].dz = (double) points[1].z * invMax;

			points[2].u  = (double) points[2].x * invWidth;
			points[2].v  = (double) points[2].y * invHeight;
			points[2].dz = (double) points[2].z * invMax;

			vector<Point> n_points = splitTriangleDraw(img, points[0], points[1], points[2], &m_delaunay_image);
			for(uint i = 0; i < n_points.size(); i += 3) {
				line(m_mesh_image, n_points[i].toCVPoint()*4.0, n_points[i+1].toCVPoint()*4.0, m_color_edge, 1, CV_AA, 0);
				line(m_mesh_image, n_points[i+1].toCVPoint()*4.0, n_points[i+2].toCVPoint()*4.0, m_color_edge, 1, CV_AA, 0);
				line(m_mesh_image, n_points[i+2].toCVPoint()*4.0, n_points[i].toCVPoint()*4.0, m_color_edge, 1, CV_AA, 0);
			}
		}
	}

	cv::imwrite("delaunay.png", m_delaunay_image);
	cv::imwrite("mesh.png", m_mesh_image);
	cv::imwrite("gx.png", m_sobel_x_image);
	cv::imwrite("gy.png", m_sobel_y_image);
}

// create index with a vector, super slow
int Compressor::getIndex(vector<cv::Point>& vertices, cv::Point p) {
	int size = vertices.size();
	for(int i = 0; i < size; ++i) {
		if(vertices[i].x == p.x && vertices[i].y == p.y)
			return i;
	}

	vertices.push_back(p);
	return size;
}

// with a map the calculation of the indice can be done much faster
int Compressor::getIndex(map<Point, int>& vertices, const Point& p) {
	int size = vertices.size();
	auto ret  = vertices.insert(pair<Point, int>(p, size));
	return ret.first->second;
}

// this method performs the delaunay triangulation with opencv
// a list of triangles will be returned
vector<cv::Vec6f> Compressor::delaunay(cv::Mat& img, list<cv::Point2f>& seeds) {
	cv::Rect rect(0, 0, img.rows, img.cols);
	cv::Subdiv2D subdiv(rect);

	vector<cv::Point2f> _seeds(seeds.begin(), seeds.end());
	subdiv.insert(_seeds);

	vector<cv::Vec6f> triangleList;
    subdiv.getTriangleList(triangleList);

	return triangleList;
}

// generating the seed points with floydSteinberg, need the gradient images and the both params
list<cv::Point2f> Compressor::floydSteinberg(cv::Mat& gx, cv::Mat& gy, double T, double gamma) {
	list<cv::Point2f> seeds;
	double M = (double)(USHRT_MAX);
	double A = sqrt(2 *(M*M));

	// create the feature map:
	// sigma(x) = (|| \nabla f(x) || / A )^gamma
	double g_x;
	double g_y;
	for(int y = 0; y < gx.cols; ++y) {
		for(int x = 0; x < gx.rows; ++x) {
			g_x =(double) gx.at<ushort>(y, x);
			g_y =(double) gy.at<ushort>(y, x);
		
			m_feature_image.at<double>(y, x) = pow( (sqrt((g_x*g_x) + (g_y*g_y)) / A), gamma);
		}
	}

	// performs the floydSteinberg dithering
	int ws = gx.cols-1;
	int hs = gx.rows-1;
	int xi, x_i, yi;
	double o, n, e;
	for(int y = 1; y < ws; ++y) {					// a border of one pixel will be created while calculating 
		for(int x = 1; x < hs; ++x) {				// the sobel gradients, we dont need to check them
			o = m_feature_image.at<double>(y, x);

			// if the value of the feature map, the pixel will be used as 
			// a seed point
			if(o > T) {								
				n = T;

				cv::Point2f p(x, y);
				seeds.push_back(p);
			}
			else n = 0.0;
      
	  		// here the error will be calculated and distributed
	  		m_feature_image.at<double>(y, x) = n;
	  		e = o - n;

			xi  = x+1;
			x_i = x-1;
			yi	= y+1;
		
			m_feature_image.at<double>(y  , xi) = m_feature_image.at<double>(y  , xi) + e * 7 / 16;
			m_feature_image.at<double>(yi, x_i) = m_feature_image.at<double>(yi, x_i) + e * 3 / 16;
			m_feature_image.at<double>(yi,   x) = m_feature_image.at<double>(yi,   x) + e * 5 / 16;
			m_feature_image.at<double>(yi, xi) = m_feature_image.at<double>(yi, xi) + e * 1 / 16;
		}
	}
	
	return seeds;
}

// resize the quadtree
void Compressor::resizeQuadtree() {
	SAFE_DELETE(m_quadtree);
	m_quadtree = new QuadTree( 	m_config->getMeshWidth(), 
								m_config->getMeshHeight(),
								m_config->getMaxDepth());
}

// check if a edge is valid, Epsilon = 1000 (ushort) 
bool Compressor::isValid(cv::Mat& img, const Edge& e) {
	Point pm = e.getPM(img, m_config->getMeshWidth(), m_config->getMeshHeight());
	if(abs(calcIntPMDepth(e) - (int)pm.z) > 1000) return false;
	
	double value_a = fabs(e.a.dz - e.b.dz);
	double value_b = norm(e.a, e.b) * (e.a.dz + e.b.dz);
	if(value_b == 0.0) return false;
	return (value_a / value_b) < m_config->getTangle();
}

// check if e is joinable
bool Compressor::isJoinable(cv::Mat& img, const Edge& e) {
	Point pm = e.getPM(img, m_config->getMeshWidth(), m_config->getMeshHeight());	
	double a = e.b.dz - pm.dz;
	double b = pm.dz  - e.a.dz;

	return fabs(a - b) < m_config->getTjoin();
}


// check if vertices of a triangle are collinear 
bool Compressor::isCollinear(const Point& p1, const Point& p2, const Point& p3) {
	if((p1.x * (p2.y - p3.y) + p2.x * (p3.y - p1.y) + p3.x * (p1.y - p2.y)) == 0)
		return true;
	return false;
}


// splitts a tringle, performs the refinement step
vector<Point> Compressor::splitTriangle(cv::Mat& img, const Point& a, const Point& b, const Point& c) {
	// resulting triangles
	vector<Point> result;

	// create edges from points
	Edge ab(a, b);
	Edge bc(b, c);
	Edge ca(c, a);

	// check if the refinement should be done
	if(m_config->getRefine()) {
		vector<Edge*> invalid_edges;
		int			  valid_edge = -1;
		
		// check for valid edges
		if(isValid(img, ab)) valid_edge = 0;
		else invalid_edges.push_back(&ab);
		if(isValid(img, bc)) valid_edge = 1;
		else invalid_edges.push_back(&bc);
		if(isValid(img, ca)) valid_edge = 2;
		invalid_edges.push_back(&ca);
		
		int i_size = invalid_edges.size();
		
		// if all edges are invalid all edges will be splitted
		if(i_size == 3) {
			// the new vector triangles is created, because some triangles
			// will not insert if they are collinear.
			vector<Point> triangles(9);
			triangles[0] = a;
			triangles[1] = getMaxJoinable(img, a, b);
			triangles[2] = getMaxJoinable(img, a, c);

			triangles[3] = b;
			triangles[4] = getMaxJoinable(img, b, c);
			triangles[5] = getMaxJoinable(img, b, a);

			triangles[6] = c;
			triangles[7] = getMaxJoinable(img, c, a);
			triangles[8] = getMaxJoinable(img, c, b);
			
			// check the created triangles if they are collinear
			if(!isCollinear(triangles[0], triangles[1], triangles[2])) {
				result.push_back(triangles[0]);
				result.push_back(triangles[1]);
				result.push_back(triangles[2]);
			}
			if(!isCollinear(triangles[3], triangles[4], triangles[5])) {
				result.push_back(triangles[3]);
				result.push_back(triangles[4]);
				result.push_back(triangles[5]);
			}
			if(!isCollinear(triangles[6], triangles[7], triangles[8])) {
				result.push_back(triangles[6]);
				result.push_back(triangles[7]);
				result.push_back(triangles[8]);
			}
		}
		else if(i_size == 2) {
			vector<Point> triangles(9);
			double la, lb;
			
			// In case of two invalid edges, 3 cases will be distinguished.
			// Another posibilty could be done by an reordering of a,b and c.
			if(valid_edge == 0) {
				triangles[0] = c;
				triangles[1] = getMaxJoinable(img, c, a);
				triangles[2] = getMaxJoinable(img, c, b);

				pair<Point, Point> p = getMaxJoinables(img, b, a, c);
				la = squareLength(a, p.first);
				lb = squareLength(b, p.second);
				if(la < lb) {
					triangles[3] = a;
					triangles[4] = p.first;
					triangles[5] = p.second;

					triangles[6] = b;
					triangles[7] = p.first;
					triangles[8] = a;
				} else {
					triangles[3] = a;
					triangles[4] = b;
					triangles[5] = p.second;

					triangles[6] = b;
					triangles[7] = p.first;
					triangles[8] = p.second;
				}
			}
			else if(valid_edge == 1) {
				triangles[0] = a;
				triangles[1] = getMaxJoinable(img, a, b);
				triangles[2] = getMaxJoinable(img, a, c);

				pair<Point, Point> p = getMaxJoinables(img, c, b, a);
				la = squareLength(b, p.first);
				lb = squareLength(c, p.second);
				if(la < lb) {
					triangles[3] = b;
					triangles[4] = p.first;
					triangles[5] = p.second;

					triangles[6] = c;
					triangles[7] = p.first;
					triangles[8] = b;
				} else {
					triangles[3] = b;
					triangles[4] = c;
					triangles[5] = p.second;

					triangles[6] = c;
					triangles[7] = p.first;
					triangles[8] = p.second;
				}
			}
			else if(valid_edge == 2) {
				triangles[0] = b;
				triangles[1] = getMaxJoinable(img, b, c);
				triangles[2] = getMaxJoinable(img, b, a);

				pair<Point, Point> p = getMaxJoinables(img, a, c, b);
				la = squareLength(c, p.first);
				lb = squareLength(a, p.second);
				if(la < lb) {
					triangles[3] = c;
					triangles[4] = p.first;
					triangles[5] = p.second;

					triangles[6] = a;
					triangles[7] = p.first;
					triangles[8] = c;
				} else {
					triangles[3] = c;
					triangles[4] = a;
					triangles[5] = p.second;

					triangles[6] = a;
					triangles[7] = p.first;
					triangles[8] = p.second;
				}
			}

			if(!isCollinear(triangles[0], triangles[1], triangles[2])) {
				result.push_back(triangles[0]);
				result.push_back(triangles[1]);
				result.push_back(triangles[2]);
			}
			if(!isCollinear(triangles[3], triangles[4], triangles[5])) {
				result.push_back(triangles[3]);
				result.push_back(triangles[4]);
				result.push_back(triangles[5]);
			}
			if(!isCollinear(triangles[6], triangles[7], triangles[8])) {
				result.push_back(triangles[6]);
				result.push_back(triangles[7]);
				result.push_back(triangles[8]);
			}
		} else { // no valid edges exits
			result.resize(3);
			result[0] = a;
			result[1] = b;
			result[2] = c;
		}
	} else { // no refinement step direct return the points
		result.resize(3);
		result[0] = a;
		result[1] = b;
		result[2] = c;
	}

	return result;
}

// done the same like splitTriangle but draws the different steps to an image
vector<Point> Compressor::splitTriangleDraw(cv::Mat& img, const Point& a, const Point& b, const Point& c, cv::Mat* out) {
	vector<Point> result;

	Edge ab(a, b);
	Edge bc(b, c);
	Edge ca(c, a);

	if(out == nullptr)
		return result;

	if(m_config->getRefine()) {
		vector<Edge*> invalid_edges;
		int			  valid_edge = -1;
		
		if(isValid(img, ab)) {
			valid_edge = 0;
			line(*out, a.toCVPoint()*4.0, b.toCVPoint()*4.0, m_color_edge, 1, CV_AA, 0);
		} else {
			invalid_edges.push_back(&ab);
			line(*out, a.toCVPoint()*4.0, b.toCVPoint()*4.0, m_color_red, 1, CV_AA, 0);
		}
		if(isValid(img, bc)) {
			valid_edge = 1;
			line(*out, b.toCVPoint()*4.0, c.toCVPoint()*4.0, m_color_edge, 1, CV_AA, 0);
		} else {
			invalid_edges.push_back(&bc);
			line(*out, b.toCVPoint()*4.0, c.toCVPoint()*4.0, m_color_red, 1, CV_AA, 0);
		}
		if(isValid(img, ca)) {
			valid_edge = 2;
			line(*out, c.toCVPoint()*4.0, a.toCVPoint()*4.0, m_color_edge, 1, CV_AA, 0);
		} else {
			invalid_edges.push_back(&ca);
			line(*out, c.toCVPoint()*4.0, a.toCVPoint()*4.0, m_color_red, 1, CV_AA, 0);
		}
		
		int i_size = invalid_edges.size();
		if(i_size == 3) {
			vector<Point> triangles(9);
			triangles[0] = a;
			triangles[1] = getMaxJoinable(img, a, b);
			triangles[2] = getMaxJoinable(img, a, c);

			triangles[3] = b;
			triangles[4] = getMaxJoinable(img, b, c);
			triangles[5] = getMaxJoinable(img, b, a);

			triangles[6] = c;
			triangles[7] = getMaxJoinable(img, c, a);
			triangles[8] = getMaxJoinable(img, c, b);
		
			if(!isCollinear(triangles[0], triangles[1], triangles[2])) {
				result.push_back(triangles[0]);
				result.push_back(triangles[1]);
				result.push_back(triangles[2]);

				line(*out, triangles[0].toCVPoint()*4.0, triangles[1].toCVPoint()*4.0, m_color_green, 1, CV_AA, 0);
				line(*out, triangles[1].toCVPoint()*4.0, triangles[2].toCVPoint()*4.0, m_color_green, 1, CV_AA, 0);
				line(*out, triangles[2].toCVPoint()*4.0, triangles[0].toCVPoint()*4.0, m_color_green, 1, CV_AA, 0);
			} else {
				line(*out, triangles[0].toCVPoint()*4.0, triangles[1].toCVPoint()*4.0, m_color_black, 1, CV_AA, 0);
				line(*out, triangles[1].toCVPoint()*4.0, triangles[2].toCVPoint()*4.0, m_color_black, 1, CV_AA, 0);
				line(*out, triangles[2].toCVPoint()*4.0, triangles[0].toCVPoint()*4.0, m_color_black, 1, CV_AA, 0);
			}
			if(!isCollinear(triangles[3], triangles[4], triangles[5])) {
				result.push_back(triangles[3]);
				result.push_back(triangles[4]);
				result.push_back(triangles[5]);

				line(*out, triangles[3].toCVPoint()*4.0, triangles[4].toCVPoint()*4.0, m_color_green, 1, CV_AA, 0);
				line(*out, triangles[4].toCVPoint()*4.0, triangles[5].toCVPoint()*4.0, m_color_green, 1, CV_AA, 0);
				line(*out, triangles[5].toCVPoint()*4.0, triangles[3].toCVPoint()*4.0, m_color_green, 1, CV_AA, 0);
			} else {
				line(*out, triangles[3].toCVPoint()*4.0, triangles[4].toCVPoint()*4.0, m_color_black, 1, CV_AA, 0);
				line(*out, triangles[4].toCVPoint()*4.0, triangles[5].toCVPoint()*4.0, m_color_black, 1, CV_AA, 0);
				line(*out, triangles[5].toCVPoint()*4.0, triangles[3].toCVPoint()*4.0, m_color_black, 1, CV_AA, 0);
			}
			if(!isCollinear(triangles[6], triangles[7], triangles[8])) {
				result.push_back(triangles[6]);
				result.push_back(triangles[7]);
				result.push_back(triangles[8]);

				line(*out, triangles[6].toCVPoint()*4.0, triangles[7].toCVPoint()*4.0, m_color_green, 1, CV_AA, 0);
				line(*out, triangles[7].toCVPoint()*4.0, triangles[8].toCVPoint()*4.0, m_color_green, 1, CV_AA, 0);
				line(*out, triangles[8].toCVPoint()*4.0, triangles[6].toCVPoint()*4.0, m_color_green, 1, CV_AA, 0);
			} else {
				line(*out, triangles[6].toCVPoint()*4.0, triangles[7].toCVPoint()*4.0, m_color_black, 1, CV_AA, 0);
				line(*out, triangles[7].toCVPoint()*4.0, triangles[8].toCVPoint()*4.0, m_color_black, 1, CV_AA, 0);
				line(*out, triangles[8].toCVPoint()*4.0, triangles[6].toCVPoint()*4.0, m_color_black, 1, CV_AA, 0);
			}
		}
		else if(i_size == 2) {
			vector<Point> triangles(9);
			double la, lb;
			
			if(valid_edge == 0) {
				triangles[0] = c;
				triangles[1] = getMaxJoinable(img, c, a);
				triangles[2] = getMaxJoinable(img, c, b);

				pair<Point, Point> p = getMaxJoinables(img, b, a, c);
				la = squareLength(a, p.first);
				lb = squareLength(b, p.second);
				if(la < lb) {
					triangles[3] = a;
					triangles[4] = p.first;
					triangles[5] = p.second;

					triangles[6] = b;
					triangles[7] = p.first;
					triangles[8] = a;
				} else {
					triangles[3] = a;
					triangles[4] = b;
					triangles[5] = p.second;

					triangles[6] = b;
					triangles[7] = p.first;
					triangles[8] = p.second;
				}
			}
			else if(valid_edge == 1) {
				triangles[0] = a;
				triangles[1] = getMaxJoinable(img, a, b);
				triangles[2] = getMaxJoinable(img, a, c);

				pair<Point, Point> p = getMaxJoinables(img, c, b, a);
				la = squareLength(b, p.first);
				lb = squareLength(c, p.second);
				if(la < lb) {
					triangles[3] = b;
					triangles[4] = p.first;
					triangles[5] = p.second;

					triangles[6] = c;
					triangles[7] = p.first;
					triangles[8] = b;
				} else {
					triangles[3] = b;
					triangles[4] = c;
					triangles[5] = p.second;

					triangles[6] = c;
					triangles[7] = p.first;
					triangles[8] = p.second;
				}
			}
			else if(valid_edge == 2) {
				triangles[0] = b;
				triangles[1] = getMaxJoinable(img, b, c);
				triangles[2] = getMaxJoinable(img, b, a);

				pair<Point, Point> p = getMaxJoinables(img, a, c, b);
				la = squareLength(c, p.first);
				lb = squareLength(a, p.second);
				if(la < lb) {
					triangles[3] = c;
					triangles[4] = p.first;
					triangles[5] = p.second;

					triangles[6] = a;
					triangles[7] = p.first;
					triangles[8] = c;
				} else {
					triangles[3] = c;
					triangles[4] = a;
					triangles[5] = p.second;

					triangles[6] = a;
					triangles[7] = p.first;
					triangles[8] = p.second;
				}
			}

			if(!isCollinear(triangles[0], triangles[1], triangles[2])) {
				result.push_back(triangles[0]);
				result.push_back(triangles[1]);
				result.push_back(triangles[2]);

				line(*out, triangles[0].toCVPoint()*4.0, triangles[1].toCVPoint()*4.0, m_color_blue, 1, CV_AA, 0);
				line(*out, triangles[1].toCVPoint()*4.0, triangles[2].toCVPoint()*4.0, m_color_blue, 1, CV_AA, 0);
				line(*out, triangles[2].toCVPoint()*4.0, triangles[0].toCVPoint()*4.0, m_color_blue, 1, CV_AA, 0);
			} else {
				line(*out, triangles[0].toCVPoint()*4.0, triangles[1].toCVPoint()*4.0, m_color_black, 1, CV_AA, 0);
				line(*out, triangles[1].toCVPoint()*4.0, triangles[2].toCVPoint()*4.0, m_color_black, 1, CV_AA, 0);
				line(*out, triangles[2].toCVPoint()*4.0, triangles[0].toCVPoint()*4.0, m_color_black, 1, CV_AA, 0);
			}
			if(!isCollinear(triangles[3], triangles[4], triangles[5])) {
				result.push_back(triangles[3]);
				result.push_back(triangles[4]);
				result.push_back(triangles[5]);

				line(*out, triangles[3].toCVPoint()*4.0, triangles[4].toCVPoint()*4.0, m_color_blue, 1, CV_AA, 0);
				line(*out, triangles[4].toCVPoint()*4.0, triangles[5].toCVPoint()*4.0, m_color_blue, 1, CV_AA, 0);
				line(*out, triangles[5].toCVPoint()*4.0, triangles[3].toCVPoint()*4.0, m_color_blue, 1, CV_AA, 0);
			} else {
				line(*out, triangles[3].toCVPoint()*4.0, triangles[4].toCVPoint()*4.0, m_color_black, 1, CV_AA, 0);
				line(*out, triangles[4].toCVPoint()*4.0, triangles[5].toCVPoint()*4.0, m_color_black, 1, CV_AA, 0);
				line(*out, triangles[5].toCVPoint()*4.0, triangles[3].toCVPoint()*4.0, m_color_black, 1, CV_AA, 0);
			}
			if(!isCollinear(triangles[6], triangles[7], triangles[8])) {
				result.push_back(triangles[6]);
				result.push_back(triangles[7]);
				result.push_back(triangles[8]);

				line(*out, triangles[6].toCVPoint()*4.0, triangles[7].toCVPoint()*4.0, m_color_blue, 1, CV_AA, 0);
				line(*out, triangles[7].toCVPoint()*4.0, triangles[8].toCVPoint()*4.0, m_color_blue, 1, CV_AA, 0);
				line(*out, triangles[8].toCVPoint()*4.0, triangles[6].toCVPoint()*4.0, m_color_blue, 1, CV_AA, 0);
			} else {
				line(*out, triangles[6].toCVPoint()*4.0, triangles[7].toCVPoint()*4.0, m_color_black, 1, CV_AA, 0);
				line(*out, triangles[7].toCVPoint()*4.0, triangles[8].toCVPoint()*4.0, m_color_black, 1, CV_AA, 0);
				line(*out, triangles[8].toCVPoint()*4.0, triangles[6].toCVPoint()*4.0, m_color_black, 1, CV_AA, 0);
			}
		} else {
			result.resize(3);
			result[0] = a;
			result[1] = b;
			result[2] = c;
		}
	} else {
		result.resize(3);
		result[0] = a;
		result[1] = b;
		result[2] = c;

		line(*out, a.toCVPoint()*4.0, b.toCVPoint()*4.0, m_color_edge, 1, CV_AA, 0);
		line(*out, b.toCVPoint()*4.0, c.toCVPoint()*4.0, m_color_edge, 1, CV_AA, 0);
		line(*out, c.toCVPoint()*4.0, a.toCVPoint()*4.0, m_color_edge, 1, CV_AA, 0);
	}
		

	return result;
}

// this method calculates the nears and best points from p to a
Point Compressor::getMaxJoinable(cv::Mat& img, const Point& p, const Point& a) {
	Point m = p;
	Point n;
	Point d = a - p;

	double step = 1.0 / length(a, p);
	for(double i = 0.0; i <= 1.0; i += step) {
		n = p + (d * i);
		n.setP(img, m_config->getMeshWidth(), m_config->getMeshHeight());

		if(isJoinable(img, Edge(p, n))) {
			m = n;
		} else {
			return m;
		}
	}

	return m;
}

// creates a list of all possible joinable possitions from p to a
vector<Point> Compressor::getMaxJoinablePoints(cv::Mat& img, const Point& p, const Point& a) {
	vector<Point> res;
	res.push_back(p);
	Point n;
	Point d = a - p;
	
	double step = 1.0 / length(a, p);
	for(double i = 0.0; i <= 1.0; i += step) {
		n = p + (d * i);
		n.setP(img, m_config->getMeshWidth(), m_config->getMeshHeight());


		if(isJoinable(img, Edge(p, n))) {
			res.push_back(n);
		} else {
			return res;
		}
	}

	return res;
}

// finds the best joinable points (from a to c) and (from b to c) while both points
// has to be joinable two
pair<Point, Point> Compressor::getMaxJoinables(cv::Mat& img, const Point& a, const Point& b, const Point& c) {
	list<pair<int, Edge> > l;

	vector<Point> v1 = getMaxJoinablePoints(img, a, c);
	vector<Point> v2 = getMaxJoinablePoints(img, b, c);

	uint size_1 = v1.size();
	uint size_2 = v2.size();

	for(uint i1 = 0; i1 < size_1; ++i1) {
		for(uint i2 = 0; i2 < size_2; ++i2) {

			Edge e(v1[i1], v2[i2]);

			
			// add to list length
			if(isJoinable(img, e)) {
				l.push_back(make_pair(i1 + i2, e));
			}
		}
	}

	l.sort([](const pair<int, Edge>& _p1, const pair<int, Edge>& _p2){
		if(_p1.first > _p2.first)
			return true;
		return false;
	});


	if(l.size() == 0)
		l.push_back(make_pair(0, Edge(a, b)));

	Edge e_r = l.begin()->second;
	pair<Point, Point> r;
	r.first = e_r.a;
	r.second = e_r.b;
	return r;
}

// getter methods for time and images
cv::Mat* Compressor::getDelaunayImage() {
	return &m_delaunay_image;
}

cv::Mat* Compressor::getMeshImage() {
	return &m_mesh_image;
}

cv::Mat* Compressor::getSeedImage() {
	return &m_seed_image;
}

cv::Mat* Compressor::getSobelXImage() {
	return &m_sobel_x_image;
}

cv::Mat* Compressor::getSobelYImage() {
	return &m_sobel_y_image;
}

cv::Mat* Compressor::getFeatureImage() {
	return &m_feature_image;
}

clock_t	Compressor::getCTimeTexture() {
	return m_ctime_texture;
}

clock_t	Compressor::getCTimeFull() {
	return m_ctime_full;
}

clock_t	Compressor::getCTimeSeeds() {
	return m_ctime_seeds;
}

clock_t	Compressor::getCTimeDelaunay() {
	return m_ctime_delaunay;
}

clock_t	Compressor::getCTimeTranform() {
	return m_ctime_transform;
}

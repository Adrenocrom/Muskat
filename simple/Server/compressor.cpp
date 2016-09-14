#include "muskat.h"

Compressor::Compressor(Config* config) {
	m_config = config;

	// generate quadtree, needs to be recreated after resize
	m_quadtree = nullptr;
	m_quadtree = new QuadTree(	m_config->getWidth(),
								m_config->getHeight(),
								m_config->getMaxDepth());
}

Compressor::~Compressor() {
	SAFE_DELETE(m_quadtree);
}

void Compressor::compressTexture(QJsonObject& jo, FrameBuffer& fb) {
	std::vector<uchar> rgb;	//buffer for coding
	std::vector<int> param(2);

	if(m_config->textureCompressionMethod == "jpeg") {
		param[0] = cv::IMWRITE_JPEG_QUALITY;
		param[1] = m_config->textureCompressionQuality;	//default(95) 0-100
		cv::imencode(".jpg", fb.rgb, rgb, param);
	}
	else if(m_config->textureCompressionMethod == "png") {
		param[0] = cv::IMWRITE_PNG_COMPRESSION;
		param[1] = m_config->textureCompressionQuality;	//default(3) 0-9
		cv::imencode(".png", fb.rgb, rgb, param);
	}
		
	QByteArray ba_rgb;
	ba_rgb.append((const char*)rgb.data(), rgb.size());

	jo["rgb"] = QString(ba_rgb.toBase64());
}

void Compressor::compressMesh(QJsonObject& jo, FrameBuffer& fb) {
	if(m_config->meshCompressionMethod == "8bit") {
		compressMesh8Bit(jo, fb);
	}
	else if(m_config->meshCompressionMethod == "16bit") {
		compressMesh16Bit(jo, fb);
	}
	else if(m_config->meshCompressionMethod == "delaunay") {
		compressMeshDelaunay(jo, fb);
	}
}

void Compressor::compressMesh8Bit(QJsonObject& jo, FrameBuffer& fb) {
	std::vector<uchar> 	depth;
	std::vector<int> 	param(2);
	
	param[0] = cv::IMWRITE_PNG_COMPRESSION;
	param[1] = 1;//default(3) 0-9
	cv::imencode(".png", fb.depth, depth, param);
	
	QByteArray ba_depth;
	ba_depth.append((const char*)depth.data(), depth.size());

	jo["depth"] = QString(ba_depth.toBase64());
}

void Compressor::compressMesh16Bit(QJsonObject& jo, FrameBuffer& fb) {
	std::vector<uchar> 	depth;
	std::vector<int> 	param(2);

	cv::Mat rgbDepth(fb.depth.rows, fb.depth.cols, CV_8UC3);
	ushort* ptr = reinterpret_cast<ushort*>(fb.depth.data);
	for(int y = 0; y < fb.depth.cols; ++y) {
		for(int x = 0; x < fb.depth.rows; ++x) {
			uchar *p = (uchar*)ptr;

			rgbDepth.at<cv::Vec3b>(y,x)[0] = 0;		// B
			rgbDepth.at<cv::Vec3b>(y,x)[1] = p[0];	// G
			rgbDepth.at<cv::Vec3b>(y,x)[2] = p[1];	// R

			ptr++;
		}
	}
	
	param[0] = cv::IMWRITE_PNG_COMPRESSION;
	param[1] = m_config->meshCompressionQuality;	//default(3) 0-9
	cv::imencode(".png", rgbDepth, depth, param);
	
	QByteArray ba_depth;
	ba_depth.append((const char*)depth.data(), depth.size());

	jo["depth"] = QString(ba_depth.toBase64());
}

void Compressor::compressMeshDelaunay(QJsonObject& jo, FrameBuffer& fb) {
	cv::Mat grad_x; 
	cv::Mat grad_y;

	// calc sobel gradients for 2 dimensions and store them
	Sobel( fb.depth, grad_x, -1, 1, 0, 3, 1, 0, cv::BORDER_DEFAULT );
	Sobel( fb.depth, grad_y, -1, 0, 1, 3, 1, 0, cv::BORDER_DEFAULT );

	// calc seeds from quadtree
	vector<cv::Point2f>	seeds = m_quadtree->generateSeeds(&grad_x, &grad_y);

	vector<cv::Vec6f> triangles = delaunay(fb.depth, seeds);

	// TODO refine
	
	vector<cv::Point> pt(3);
	vector<cv::Point> vertices;
	
	cv::Size size = fb.depth.size();
	cv::Rect rect(0,0, size.width, size.height);

	cv::Mat meshImage(512, 512, CV_8UC3, cv::Scalar(255,255,255));

	cv::Scalar 	delaunay_color(255,0,0);
	cv::Scalar 	color(0,0,255);

	QJsonArray ja_indices;
	QJsonArray ja_vertices;

	uint t_size = triangles.size();
	for(uint i = 0; i < triangles.size(); ++i ) {
		cv::Vec6f t = triangles[i];
        pt[0] = cv::Point(cvRound(t[0]), cvRound(t[1]));
        pt[1] = cv::Point(cvRound(t[2]), cvRound(t[3]));
        pt[2] = cv::Point(cvRound(t[4]), cvRound(t[5]));

		if ( rect.contains(pt[0]) && rect.contains(pt[1]) && rect.contains(pt[2])) {

			// Draw delaunay triangles

			line(meshImage, pt[0], pt[1], delaunay_color, 1, CV_AA, 0);
            line(meshImage, pt[1], pt[2], delaunay_color, 1, CV_AA, 0);
            line(meshImage, pt[2], pt[0], delaunay_color, 1, CV_AA, 0);

 			circle( meshImage, pt[0], 2, color, CV_FILLED, CV_AA, 0 );
 			circle( meshImage, pt[1], 2, color, CV_FILLED, CV_AA, 0 );
 			circle( meshImage, pt[2], 2, color, CV_FILLED, CV_AA, 0 );

			ja_indices.push_back((int)getIndex(vertices, pt[0]));
			ja_indices.push_back((int)getIndex(vertices, pt[1]));
			ja_indices.push_back((int)getIndex(vertices, pt[2]));
		}
	}
	
	uint v_size = vertices.size();
	int num_texCoord = 0;
	for(uint i = 0; i < vertices.size(); ++i) {
		ja_vertices.push_back((float)vertices[i].x / (float)511);
		ja_vertices.push_back((float)vertices[i].y / (float)511);
		ja_vertices.push_back( (float)(fb.depth.at<ushort>(vertices[i].x, vertices[i].y)) / (float)(USHRT_MAX));
	
		num_texCoord += 2;
	}
    
	imwrite("res/delaunay.png", meshImage);

	jo["indices"] 	= ja_indices;
	jo["vertices"] 	= ja_vertices;
	jo["numTexCoord"] = num_texCoord;
}

uint Compressor::getIndex(vector<cv::Point>& vertices, cv::Point p) {
	uint size = vertices.size();
	for(uint i = 0; i < size; ++i) {
		if(vertices[i].x == p.x && vertices[i].y == p.y)
			return i;
	}

	vertices.push_back(p);
	return size;
}

QJsonObject Compressor::compressFrame(FrameInfo& info, FrameBuffer& fb) {
	QJsonObject jo;

	QMatrix4x4 perspective;
	QMatrix4x4 camera;

	perspective.perspective(info.offangle, 1.0, info.near, info.far);
	camera.lookAt(info.pos, info.lookAt, info.up);

	QMatrix4x4 modelView 	= perspective * camera;
	QMatrix4x4 invModelView = modelView.inverted();

	compressTexture(jo, fb);
	compressMesh(jo, fb);

	QJsonArray ja;
	for(uint i = 0; i < 16; ++i)
		ja.push_back(invModelView.data()[i]);

	jo["invMVP"] = ja;
	return jo;
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

bool Compressor::testAngle(cv::Mat& img, Edge& e, int T_angle) {
	double value_a = abs(img.at<ushort>(e.p1.y, e.p1.x) - img.at<ushort>(e.p2.y, e.p2.x));
	double value_b = norm(e.p1 - e.p2) * (img.at<ushort>(e.p1.y, e.p1.x) + img.at<ushort>(e.p2.y, e.p2.x));

	return (value_a / value_b) < T_angle;
}

bool Compressor::testJoinable(cv::Mat& img, Edge& e, int T_join) {
	cv::Point pm = cv::Point(cvRound((e.p1.x + e.p2.x) / 2), cvRound((e.p1.y + e.p2.y) / 2));

	double a = (img.at<ushort>(e.p2.y, e.p2.x) - img.at<ushort>(pm.y, pm.x)); 
	double b = (img.at<ushort>(pm.y, pm.x) - img.at<ushort>(e.p1.y, e.p1.x)); 

	return abs(a - b) < T_join;
}

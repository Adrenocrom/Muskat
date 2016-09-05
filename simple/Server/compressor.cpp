#include "muskat.h"

Compressor::Compressor(Config* config) {
	m_config = config;
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
	cv::Mat grad_x, grad_y;

	// Gradient X
	Sobel( fb.depth, grad_x, -1, 1, 0, 3, 1, 0, cv::BORDER_DEFAULT );
	
	// Gradient Y
	Sobel( fb.depth, grad_y, -1, 0, 1, 3, 1, 0, cv::BORDER_DEFAULT );
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

QuadTree::QuadTree(uint w, uint h, uint max_depth) {
	QTNode	 root;
	root.x = 0;
	root.y = 0;
	root.w = w;
	root.h = h;
	root.parent = -1;
	nodes.push_back(root);
	createChildren(0, 1, max_depth);
	
	hnodes.resize(max_depth-1);
	hnodes[0].push_back(0);
}

void QuadTree::createChildren(uint pid, uint current_depth, uint max_depth) {
	QTNode& parent = nodes[pid];
	
	if(current_depth >= max_depth) {
		parent.l1 = -1;
		parent.l2 = -1;
		parent.l3 = -1;
		parent.l4 = -1;
		parent.isLeaf = true;
		leafs.push_back(pid);
	} else {
		uint wh = (uint)((float)parent.w / 2.0f);
		uint hh = (uint)((float)parent.h / 2.0f);

		QTNode l1;
		l1.x = parent.x;
		l1.y = parent.y;
		l1.w = wh-1;
		l1.h = hh-1;
		parent.l1 = nodes.size();
		hnodes[current_depth].push_back(parent.l1);
		nodes.push_back(l1);

		QTNode l2;
		l2.x = wh;
		l2.y = parent.y;
		l2.w = parent.w;
		l2.h = hh-1;
		parent.l2 = nodes.size();
		hnodes[current_depth].push_back(parent.l2);
		nodes.push_back(l2);

		QTNode l3;
		l3.x = parent.x;
		l3.y = hh;
		l3.w = wh-1;
		l3.h = parent.h;
		parent.l3 = nodes.size();
		hnodes[current_depth].push_back(parent.l3);
		nodes.push_back(l3);

		QTNode l4;
		l4.x = wh;
		l4.y = hh;
		l4.w = parent.w;
		l4.h = parent.h;
		parent.l4 = nodes.size();
		hnodes[current_depth].push_back(parent.l4);
		nodes.push_back(l4);

		createChildren(parent.l1, current_depth+1, max_depth);
		createChildren(parent.l2, current_depth+1, max_depth);
		createChildren(parent.l3, current_depth+1, max_depth);
		createChildren(parent.l4, current_depth+1, max_depth);
	}
}

void QuadTree::calcCxyLeafs(cv::Mat& Gx, cv::Mat& Gy, int& T_leaf) {
	uint  l_size = leafs.size();
	QTNode& node = nodes[0];
	ushort max_x = 0;
	ushort min_x = USHRT_MAX;
	ushort max_y = 0;
	ushort min_y = USHRT_MAX;

	for(uint i = 0; i < l_size; ++i) {
		node = nodes[leafs[i]];

		ushort* grad_x = reinterpret_cast<ushort*>(Gx.data);
		ushort* grad_y = reinterpret_cast<ushort*>(Gy.data);
		for(uint y = node.y; y < node.h; ++y) {
			for(uint x = node.x; x < node.w; ++x) {
				if(*grad_x < min_x) min_x = *grad_x;
				if(*grad_x > max_x) max_x = *grad_x;
				
				if(*grad_y < min_y) min_y = *grad_y;
				if(*grad_y > max_y) max_y = *grad_y;

				grad_x++;
				grad_y++;
			}
		}
		
		node.Cx = max_x - min_x;
		node.Cy = max_y - min_y;
		node.H_x = max_x;
		node.H_y = max_y;
		node.L_x = min_x;
		node.L_y = min_y;

		if(node.Cx > T_leaf || node.Cy > T_leaf) {
			cv::Point2f pt1(node.x, node.y);
			cv::Point2f pt2(node.w, node.y);
			cv::Point2f pt3(node.x, node.h);
			cv::Point2f pt4(node.w, node.h);

			seeds.push_back(pt1);
			seeds.push_back(pt2);
			seeds.push_back(pt3);
			seeds.push_back(pt4);
		}
	}
}

void QuadTree::calcCxy(cv::Mat& Gx, cv::Mat& Gy, int T_internal, int T_leaf) {
	calcCxyLeafs(Gx, Gy, T_leaf);

	for(uint i = hnodes.size(); i <= 0; --i) {
		for(uint n = 0; n < hnodes[i].size(); ++n) {
			QTNode& node = nodes[(int)hnodes[i][n]];

			node.H_x = MU_MAX4(nodes[node.l1].H_x, 
							   nodes[node.l2].H_x, 
							   nodes[node.l3].H_x, 
							   nodes[node.l4].H_x);

			node.H_y = MU_MAX4(nodes[node.l1].H_y, 
							   nodes[node.l2].H_y, 
							   nodes[node.l3].H_y, 
							   nodes[node.l4].H_y);

			node.L_x = MU_MIN4(nodes[node.l1].L_x, 
							   nodes[node.l2].L_x, 
							   nodes[node.l3].L_x, 
							   nodes[node.l4].L_x);

			node.L_y = MU_MIN4(nodes[node.l1].L_y, 
							   nodes[node.l2].L_y, 
							   nodes[node.l3].L_y, 
							   nodes[node.l4].L_y);
			
			node.Cx = node.H_x - node.L_x;
			node.Cy = node.H_y - node.L_y;

			if(node.Cx > T_internal || node.Cy > T_internal) {
				cv::Point2f pt1(node.x, node.y);
				cv::Point2f pt2(node.w, node.y);
				cv::Point2f pt3(node.x, node.h);
				cv::Point2f pt4(node.w, node.h);

				seeds.push_back(pt1);
				seeds.push_back(pt2);
				seeds.push_back(pt3);
				seeds.push_back(pt4);
			}

		}
	}
}

vector<cv::Vec6f> Compressor::delaunay(cv::Mat& img, vector<cv::Point2f>& seeds, int T_angle, int T_join) {
	cv::Rect rect(0, 0, img.rows, img.cols);
	cv::Subdiv2D subdiv(rect);

	uint size = seeds.size();
	for(uint i = 0; i < size; ++i) {
		subdiv.insert(seeds[i]);
	}

	vector<cv::Point> pt(3);
	vector<cv::Vec6f> triangleList;
	vector<cv::Vec6f> resultTriangles;
    subdiv.getTriangleList(triangleList);

	Edge e[3];
	ushort cnt;
	for(uint i = 0; i < triangleList.size(); i++ ) {
        cv::Vec6f t = triangleList[i];
        pt[0] = cv::Point(cvRound(t[0]), cvRound(t[1]));
        pt[1] = cv::Point(cvRound(t[2]), cvRound(t[3]));
        pt[2] = cv::Point(cvRound(t[4]), cvRound(t[5]));
     
	 	e[0].p1 = pt[0];
	 	e[0].p2 = pt[1];
		e[1].p1 = pt[1];
		e[1].p2 = pt[2];
		e[2].p1 = pt[2];
		e[2].p2 = pt[0];

		cnt = 0;

		cnt += testAngle(img, e[0], T_angle);
		cnt += testAngle(img, e[1], T_angle);
		cnt += testAngle(img, e[2], T_angle);

		if(cnt > 2) {
			cv::Vec6f triangle;
			triangle[0] = pt[0].x;
			triangle[1] = pt[0].y;
			triangle[2] = pt[1].x;
			triangle[3] = pt[1].y;
			triangle[4] = pt[2].x;
			triangle[5] = pt[2].y;
			resultTriangles.push_back(triangle);
		} else {
			// TODO split triangles
		}
    }

	return resultTriangles;
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

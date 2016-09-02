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
		nodes.push_back(l1);

		QTNode l2;
		l2.x = wh;
		l2.y = parent.y;
		l2.w = parent.w;
		l2.h = hh-1;
		parent.l2 = nodes.size();
		nodes.push_back(l2);

		QTNode l3;
		l3.x = parent.x;
		l3.y = hh;
		l3.w = wh-1;
		l3.h = parent.h;
		parent.l3 = nodes.size();
		nodes.push_back(l3);

		QTNode l4;
		l4.x = wh;
		l4.y = hh;
		l4.w = parent.w;
		l4.h = parent.h;
		parent.l4 = nodes.size();
		nodes.push_back(l4);

		createChildren(parent.l1, current_depth+1, max_depth);
		createChildren(parent.l2, current_depth+1, max_depth);
		createChildren(parent.l3, current_depth+1, max_depth);
		createChildren(parent.l4, current_depth+1, max_depth);
	}
}

void QuadTree::calcCxyLeafs(cv::Mat& Gx, cv::Mat& Gy) {
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
	}
}

void QuadTree::calcCxy(uint pid) {
}
	/*
ushort getH_x(uint pid) {
	if(node.isLeaf)
		return node.H_x;

	
}*/

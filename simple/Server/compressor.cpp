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

		/*	bitset<16> a(*ptr);
			bitset<8>  b(p[1]);
			bitset<8>  c(p[0]);
			cout<<a<<" "<<b<<c<<endl;
*/
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

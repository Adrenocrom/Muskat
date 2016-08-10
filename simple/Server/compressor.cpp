#include "muskat.h"

QJsonObject Compressor::compressFrame(FrameInfo& info, FrameBuffer& fb) {
	QJsonObject jo;

	QMatrix4x4 perspective;
	QMatrix4x4 camera;

	perspective.perspective(info.offangle, 1.0, info.near, info.far);
	camera.lookAt(info.pos, info.lookAt, info.up);

	QMatrix4x4 modelView 	= perspective * camera;
	QMatrix4x4 invModelView = modelView.inverted();

	std::vector<uchar> rgb;	//buffer for coding
	std::vector<uchar> depth;
	std::vector<int> param(2);
	param[0] = cv::IMWRITE_JPEG_QUALITY;
	param[1] = 50;//default(95) 0-100
	cv::imencode(".jpg", fb.rgb, rgb, param);
	
	param[0] = cv::IMWRITE_PNG_COMPRESSION;
	param[1] = 1;//default(3) 0-9
	cv::imencode(".png", fb.depth, depth, param);

	QByteArray ba_rgb;
	ba_rgb.append((const char*)rgb.data(), rgb.size());

	QByteArray ba_depth;
	ba_depth.append((const char*)depth.data(), depth.size());

	QJsonArray ja;
	for(uint i = 0; i < 16; ++i)
		ja.push_back(invModelView.data()[i]);

	jo["invMVP"] = ja;
	jo["rgb"]	= QString(ba_rgb.toBase64());
	jo["depth"]	= QString(ba_depth.toBase64());
	return jo;
}

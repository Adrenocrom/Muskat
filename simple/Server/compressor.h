#ifndef COMPRESSOR_H
#define COMPRESSOR_H

#pragma once

class Compressor {
public:

	static QJsonObject compressFrame(FrameInfo& info, FrameBuffer& fb);
private:
};

#endif

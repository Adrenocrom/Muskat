#ifndef COMPRESSOR_H
#define COMPRESSOR_H

#pragma once

class Compressor {
public:

	static QJsonObject compressFrame(SFrameInfo& info, SFrameBuffer& fb);
private:
};

#endif

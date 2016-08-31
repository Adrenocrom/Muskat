#include "muskat.h"

Config::Config() {
	textureCompressionMethod 	= "jpeg";
	textureCompressionQuality 	= 0;
	meshCompressionMethod		= "8bit";
	meshCompressionQuality		= 0;
}

void Config::setConfig(QJsonObject& jo) {
	textureCompressionMethod 	= jo["textureCompressionMethod"].toString();
	textureCompressionQuality	= jo["textureCompressionQuality"].toInt();
	meshCompressionMethod		= jo["meshCompressionMethod"].toString();
	meshCompressionQuality		= jo["meshCompressionQuality"].toInt();
}

QJsonObject Config::getConfig() {
	QJsonObject jo;

	jo["textureCompressionMethod"] 	= textureCompressionMethod;
	jo["textureCompressionQuality"]	= textureCompressionQuality;
	jo["meshCompressionMethod"]		= meshCompressionMethod;
	jo["meshCompressionQuality"]	= meshCompressionQuality;

	return jo;
}

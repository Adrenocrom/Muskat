#ifndef CONFIG_H
#define CONFIG_H

#pragma once

struct Config {

	QString		textureCompressionMethod;
	int			textureCompressionQuality;
	QString		meshCompressionMethod;
	int			meshCompressionQuality;

	Config();

	QJsonObject getConfig();
	void		setConfig(QJsonObject& jo);
};

#endif

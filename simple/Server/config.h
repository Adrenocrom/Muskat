#ifndef CONFIG_H
#define CONFIG_H

#pragma once

struct Config {
	std::string renderer;

	QJsonObject getConfig();
	void		setConfig(QJsonObject& jo);
};

#endif

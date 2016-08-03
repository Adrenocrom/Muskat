#ifndef PLAYLIST_H
#define PLAYLIST_H

#pragma once

class Playlist {
public:
	string		m_dir;
	string		m_suffix;

	vector<string> 	m_file_names;
	vector<Scene> 	m_scenes;

	Playlist(string dir, string suffix);

	QJsonObject	getPlaylist();

private:
	
	std::vector<boost::filesystem::path> getFiles(boost::filesystem::path& p);
	
	void addScenes(boost::filesystem::path& p, std::string s = "_info.txt");
};

#endif

/***********************************************************
 *
 *
 *						PLAYLIST HEADER
 *					 =====================
 *
 *		AUTHOR: Josef Schulz
 *
 *		Playlist stores infos about existing scenes
 *		and is used to show them to the client.
 *
 ***********************************************************/

#ifndef PLAYLIST_H
#define PLAYLIST_H

#pragma once

class Playlist {
public:
	string		m_dir;
	string		m_suffix;

	// stores the names of the scene info files
	vector<string> 	m_file_names;

	// stores the scenes itself
	vector<Scene> 	m_scenes;

	// creates the playlist while loading the scenes
	// mainwindow pointer will be used to control
	// the progressbar
	Playlist(MainWindow* mw, string dir, string suffix);

	// create jsonobject of the playlist
	QJsonObject	getPlaylist();

private:
	
	// get all subdirectories of an path p
	std::vector<boost::filesystem::path> getFiles(boost::filesystem::path& p);
	
	// add scene infos from filesystem, dedected by the suffix
	void addScenes(boost::filesystem::path& p, std::string s = "_info.txt");
};

#endif

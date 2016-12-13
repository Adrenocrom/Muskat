/***********************************************************
 *
 *
 *						PLAYLIST SOURCE
 *					 =====================
 *
 *		AUTHOR: Josef Schulz
 *
 *		Defines the playlist class. More informations
 *		are in playlist.h
 *
 ***********************************************************/

#include "muskat.h"

using namespace boost::filesystem;

// constructer load and stores the scenes. 
// And will be used to show all scene informations to the client
Playlist::Playlist(MainWindow* mw, string dir, string suffix) {
	m_dir 		= dir;
	m_suffix 	= suffix;
	path p(m_dir);
	
	// find all scenes in the subdirs of p, dedected by the suffix
	addScenes(p, m_suffix);

	// sort list of scene info files by name
	sort(m_file_names.begin(), m_file_names.end());

	// load the scene informations, camera infos, rgb and depth images
	mw->m_progress_load->setRange(0, m_file_names.size()-1);
	for(uint i = 0; i < m_file_names.size(); ++i) {
		mw->m_progress_load->setValue(i);
		mw->repaint();
		
		Scene scene(m_file_names[i], m_suffix);
		m_scenes.push_back(scene);
	}

	// create two extra scenes of the three default runs, used for evaluation
	Scene CoolRandom("CoolRandom");
	CoolRandom.addScene(m_scenes[0]);
	CoolRandom.addScene(m_scenes[1]);
	CoolRandom.addScene(m_scenes[2]);
	Scene TestSpheres("TestSpheres");
	TestSpheres.addScene(m_scenes[3]);
	TestSpheres.addScene(m_scenes[4]);
	TestSpheres.addScene(m_scenes[5]);
	
	m_scenes.push_back(CoolRandom);
	m_scenes.push_back(TestSpheres);
}

// find scene info files in the subdirs of p, dedected with the suffix s
void Playlist::addScenes(path& p, string s) {
	vector<path> ps = getFiles(p);
	size_t t;
	uint size = ps.size();

	for(uint i = 0; i < size; ++i) {
		if(is_directory(ps[i]))
			addScenes(ps[i], s);
		else if(is_regular_file(ps[i])) {
			t = ps[i].filename().string().rfind(s);
			if(t != string::npos)
				m_file_names.push_back(ps[i].string());
		}
	}
}
	
// gathering all subdirs of path p
vector<path> Playlist::getFiles(path& p) {
	vector<path> result;

	if(exists(p) && is_directory(p)) {
		for(auto& entry : boost::make_iterator_range(directory_iterator(p), {}))
			result.push_back(entry.path());
	}


	return result;
}

// creates a jsonobject of the playlist
QJsonObject Playlist::getPlaylist() {
	QJsonObject jo_playlist;

	QJsonArray	ja_scenes;
	for(uint i = 0; i < m_scenes.size(); ++i) {
		ja_scenes.push_back(m_scenes[i].getSceneInfo());
	}

	jo_playlist["scenes"] = ja_scenes;

	return jo_playlist;
}

#include "muskat.h"

using namespace boost::filesystem;

Playlist::Playlist(MainWindow* mw, string dir, string suffix) {
	m_dir 		= dir;
	m_suffix 	= suffix;
	path p(m_dir);
	
	addScenes(p, m_suffix);

	sort(m_file_names.begin(), m_file_names.end());

	mw->m_progress_load->setRange(0, m_file_names.size()-1);

	for(uint i = 0; i < m_file_names.size(); ++i) {
		//cout<<m_file_names[i]<<endl;
		mw->m_progress_load->setValue(i);
		mw->repaint();
		
		Scene scene(m_file_names[i], m_suffix);
		m_scenes.push_back(scene);
	}
}

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
	
vector<path> Playlist::getFiles(path& p) {
	vector<path> result;

	if(exists(p) && is_directory(p)) {
		for(auto& entry : boost::make_iterator_range(directory_iterator(p), {}))
			result.push_back(entry.path());
	}


	return result;
}

QJsonObject Playlist::getPlaylist() {
	QJsonObject jo_playlist;

	QJsonArray	ja_scenes;
	for(uint i = 0; i < m_scenes.size(); ++i) {
		ja_scenes.push_back(m_scenes[i].getSceneInfo());
	}

	jo_playlist["scenes"] = ja_scenes;

	return jo_playlist;
}

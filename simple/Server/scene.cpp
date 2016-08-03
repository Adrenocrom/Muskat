#include "muskat.h"

using namespace boost::filesystem;

Scene::Scene(string filename, string suffix) {
	m_filename 	= filename;
	m_suffix	= suffix;

	m_infos.clear();
	m_fbs.clear();
	
	ifstream file(filename.c_str());
	if (!file.is_open()) {
		std::cerr << "Error: Could not find file "<<filename<<"."<<std::endl;
		return;
	}

	string str_line;
	while(getline(file, str_line, '\n')) {
		stringstream line(str_line);
		procressLine(line);
	}
	file.close();

	uint size = m_infos.size();
	path p(filename);
	string s = p.string();
	s.resize(s.size() - suffix.size());

	SFrameBuffer fb;
	for(uint i = 0; i < size; ++i) {
		string fn = s + "_" + insertZeros(5, i);

		fb.rgb	= cv::imread(fn + ".png");
		fb.depth = cv::imread(fn + "_depth.png", CV_LOAD_IMAGE_GRAYSCALE| CV_LOAD_IMAGE_ANYDEPTH);
		
		m_fbs.push_back(fb);
	}
}

void Scene::procressLine(stringstream& line) {
	string 		tag;
	string 	  	buf;
	SFrameInfo 	info;

	while(getline(line, tag, ' ')) {
		if(tag.empty()) continue;
		else if(!tag.compare("data")) continue;
		else if(!tag.compare("file")) continue;
		else if(!tag.compare("timestep")) {
			line >> buf;
			m_timestep = atoi(buf.c_str());
		}
		else if(!tag.compare("boundingbox")) {
			line >> buf; m_aabb_min.setX(atof(removeChars(buf).c_str()));
			line >> buf; m_aabb_min.setY(atof(removeChars(buf).c_str()));
			line >> buf; m_aabb_min.setZ(atof(removeChars(buf).c_str()));

			line >> buf; m_aabb_max.setX(atof(removeChars(buf).c_str()));
			line >> buf; m_aabb_max.setY(atof(removeChars(buf).c_str()));
			line >> buf; m_aabb_max.setZ(atof(removeChars(buf).c_str()));
		}
		else if(!tag.compare("aperture")) {
			line >> buf;
			m_aperture = atof(buf.c_str());
		}
		else if(!tag.compare("image")) {
			line >> buf;
			info.id = stoi(buf.c_str());
		}
		else if(!tag.compare("camera")) {
			line >> buf; 	info.pos.setX(atof(removeChars(buf).c_str()));
			line >> buf; 	info.pos.setY(atof(removeChars(buf).c_str()));
			line >> buf; 	info.pos.setZ(atof(removeChars(buf).c_str()));

			line >> buf; 	info.lookAt.setX(atof(removeChars(buf).c_str()));
			line >> buf; 	info.lookAt.setY(atof(removeChars(buf).c_str()));
			line >> buf; 	info.lookAt.setZ(atof(removeChars(buf).c_str()));

			line >> buf; 	info.up.setX(atof(removeChars(buf).c_str()));
			line >> buf; 	info.up.setY(atof(removeChars(buf).c_str()));
			line >> buf; 	info.up.setZ(atof(removeChars(buf).c_str()));
		}
		else if(!tag.compare("clipdists")) {
			line >> buf; 	info.near 	= atof(removeChars(buf).c_str());
			line >> buf; 	info.far 	= atof(removeChars(buf).c_str());
		}
		else if(!tag.compare("offangle")) {
			line >> buf; 	info.offangle 	= atof(removeChars(buf).c_str());
			
			m_infos.push_back(info);
		}
	}
}

string Scene::removeChars(string& str) {
	string res;

	for (const auto c : str)
		if(!(c == ',' || c == ' ' || c == ')' || c == '('))
			res.push_back(c);

	return res;
}

QJsonObject Scene::getSceneInfo() {
	QJsonObject jo;

	jo["name"] 		= QString::fromStdString(m_scene_name);
	jo["timestep"]	= (int) m_timestep;
	QJsonArray aabb_min;
	aabb_min.push_back(m_aabb_min.x());
	aabb_min.push_back(m_aabb_min.y());
	aabb_min.push_back(m_aabb_min.z());
	jo["aabb_min"] 	= aabb_min;
	QJsonArray aabb_max;
	aabb_max.push_back(m_aabb_max.x());
	aabb_max.push_back(m_aabb_max.y());
	aabb_max.push_back(m_aabb_max.z());
	jo["aabb_max"] 	= aabb_max;
	jo["aspect"]	= m_aspect;
	jo["aperture"]	= m_aperture;

	QJsonArray	ja;
	for(uint i = 0; i < m_infos.size(); ++i) {
		ja.push_back(getFrameInfo(i));
	}
	jo["frames"] 	= ja;

	return jo;
}

QJsonObject Scene::getFrameInfo(uint frame_id) {
	QJsonObject jo;
	SFrameInfo& fb = m_infos[frame_id];

	jo["id"] = (int)fb.id;
	QJsonArray pos;
	pos.push_back(fb.pos.x());
	pos.push_back(fb.pos.y());
	pos.push_back(fb.pos.z());
	jo["pos"] = pos;
	QJsonArray lookAt;
	lookAt.push_back(fb.lookAt.x());
	lookAt.push_back(fb.lookAt.y());
	lookAt.push_back(fb.lookAt.z());
	jo["lookat"] = lookAt;
	QJsonArray up;
	up.push_back(fb.up.x());
	up.push_back(fb.up.y());
	up.push_back(fb.up.z());
	jo["up"] 		= up;
	jo["near"] 		= fb.near;
	jo["far"] 		= fb.far;
	jo["offangle"] 	= fb.offangle;

	return jo;
}

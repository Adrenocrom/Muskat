#include "muskat.h"

using namespace boost::filesystem;

void FileRenderer::getFrame(SFrameInfo& info, SFrameBuffer& fb) {

}

void FileRenderer::loadScene(string filename, string suffix) {
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

	for(uint i = 0; i < size; ++i) {
		cv::Mat img;
		img = cv::imread()
	}
	
}

void FileRenderer::procressLine(stringstream& line) {
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
			line >> buf; m_boundingbox_min.setX(atof(removeChars(buf).c_str()));
			line >> buf; m_boundingbox_min.setY(atof(removeChars(buf).c_str()));
			line >> buf; m_boundingbox_min.setZ(atof(removeChars(buf).c_str()));

			line >> buf; m_boundingbox_max.setX(atof(removeChars(buf).c_str()));
			line >> buf; m_boundingbox_max.setY(atof(removeChars(buf).c_str()));
			line >> buf; m_boundingbox_max.setZ(atof(removeChars(buf).c_str()));
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

string FileRenderer::removeChars(string& str) {
	string res;

	for (const auto c : str)
		if(!(c == ',' || c == ' ' || c == ')' || c == '('))
			res.push_back(c);

	return res;
}

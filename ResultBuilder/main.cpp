#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <vector>
#include <list>
#include <string>
#include <algorithm>

using namespace std;

#define T_MIN -1.0

struct Entry {
	double x;
	double y;
	double z;
};

typedef vector<Entry> 	Diagram;
typedef	vector<double> 	Col;
typedef	unsigned int	uint;

Col 	loadFromTex(string filename);
void 	saveToTex(string filename, string shortname, const Diagram& diagram);

double	sumCol(const Col& col);
double	meanCol(const Col& col);

//double	getFromAngle(const Col& col, double a);

int main(int argc, char* argv[]) {
	//loadFromTex("../results/4/512x512_Delaunay/D8/L0.2/I0.2/results.tex");
	char val[20];
	
	for(uint s = 0; s < 6; ++s) {
		Diagram diagram;
		for(double l = 0.0; l <= 1.0; l += 0.1) {
			for(double i = 0.0; i <= 1.0; i += 0.1) {
			
				Entry entry;
				entry.x = l;
				entry.y = i;
				if(i <= l) {

					sprintf(val, "%d", s);
					string name = "../results/";
					name += val;
					name += "/512x512_Delaunay/D10/L";
			
					sprintf(val, "%.1f", l);
					name += val;
					name += "/I";
					sprintf(val, "%.1f", i);
					name += val;
					name += "/results.tex";

					if(s == 0) entry.z = loadFromTex(name)[10];
					if(s == 1) entry.z = loadFromTex(name)[10];
					if(s == 2) entry.z = loadFromTex(name)[10];
					if(s == 3) entry.z = loadFromTex(name)[10];
					if(s == 4) entry.z = loadFromTex(name)[10];
					if(s == 5) entry.z = loadFromTex(name)[10];
				} else {
					entry.z = -1.0;
				}

				diagram.push_back(entry);
			}
	
			Entry entry;
			entry.x = -1.0;
			entry.y = -1.0;
			entry.z = -1.0;
			diagram.push_back(entry);
		}

		string shortname;
		sprintf(val, "%d", s);
		shortname += "d_";
		shortname += val;

		string filename;
		filename += "../results/";
		filename += shortname;
		filename += ".tex";

		saveToTex(filename, shortname, diagram);
	}

	return 0;
}

Col loadFromTex(string filename) {
	Col col;
	
	ifstream file(filename.c_str());
	if(!file.is_open()) {
		cerr<<"file not found!"<<endl;
		return col;	
	}

	string str_line;
	while(getline(file, str_line, '\n')) {if(str_line == "a,p,r,g,b,m") break;}
	while(getline(file, str_line, '\n')) {if(str_line == "a,p,r,g,b,m") break;}

	string value;
	while(getline(file, str_line, '\n')) {
		if(str_line == "\\end{filecontents}") break;

		stringstream line(str_line);
		while(getline(line, value, ',')) {}
		col.push_back(std::stod(value));
	}

	file.close();
	return col;
}

double	sumCol(const Col& col) {
	uint size = col.size();
	
	double sum = 0.0;
	for(uint i = 0; i < size; ++i) {
		if(col[i] > T_MIN)
			sum += col[i];
	}

	return sum;
}

double	meanCol(const Col& col) {
	uint size = col.size();
	
	double sum = 0.0;
	for(uint i = 0; i < size; ++i) {
		if(col[i] > T_MIN)
			sum += col[i];
	}

	return sum / ((double) size);
}

void 	saveToTex(string filename, string shortname, const Diagram& diagram) {
	ofstream file;
  	file.open(filename);
	file<<"\\begin{filecontents}{div_"<<shortname<<".csv}\n";
	//file<<"a,b,c\n";

	uint size = diagram.size();
	for(uint i = 0; i < size; ++i) {
		const Entry& entry = diagram[i];

		if(entry.x == -1.0 || entry.y == -1.0 || entry.z == -1.0) {}//	file<<"\n";
		else {
			file<<entry.y<<" "<<entry.x<<" "<<entry.z<<"\n";
		}
	}
	
	file<<"\\end{filecontents}\n";

	file.close();
}

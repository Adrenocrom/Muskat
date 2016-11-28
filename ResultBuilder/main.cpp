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

Col 	loadFromTex(string filename, string shortname);
void 	saveToTex(string filename, string shortname, const Diagram& diagram);

double	sumCol(const Col& col);
double	meanCol(const Col& col);

void 	createDiagramQuadtree(uint s, uint d);
void 	createDiagramQuadtreeCost(uint s, uint d);
void 	createDiagramFloydSteinberg(uint s);
void 	createDiagramFloydSteinbergCost(uint s);

//double	getFromAngle(const Col& col, double a);

int main(int argc, char* argv[]) {
	//loadFromTex("../results/4/512x512_Delaunay/D8/L0.2/I0.2/results.tex");
	
	for(uint s = 6; s <= 7; ++s) {
		createDiagramQuadtree(s, 10);
		createDiagramQuadtree(s, 9);
		createDiagramQuadtree(s, 8);
		createDiagramQuadtreeCost(s, 10);
		createDiagramQuadtreeCost(s, 9);
		createDiagramQuadtreeCost(s, 8);
		createDiagramFloydSteinberg(s);
		createDiagramFloydSteinbergCost(s);
	}

	return 0;
}

Col loadFromTex(string filename, string shortname) {
	Col col;
	
	ifstream file(filename.c_str());
	if(!file.is_open()) {
		cerr<<"file not found!"<<endl;
		return col;	
	}

	string str_line;
	while(getline(file, str_line, '\n')) {if(str_line.compare(shortname) == 0) break;}
	getline(file, str_line, '\n');

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

void createDiagramQuadtree(uint s, uint d) {
	char val[20];
	char val_d[20];
	sprintf(val_d, "%d", d);

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
				name += "/512x512_Delaunay/D";
				name += val_d;
				name += "/L";
				string sname = "\\begin{filecontents}{div_data_mean_";
				sname += val;
				sname += "_512x512D";
				sname += val_d;
				sname += "L";
		
				sprintf(val, "%.1f", l);
				name += val;
				name += "/I";
				sname += val;
				sname += "I";
				sprintf(val, "%.1f", i);
				name += val;
				name += "/_pre/results.tex";
				sname += val;
				sname += "pre.csv}";

				entry.z = loadFromTex(name, sname)[14];
				// 10 = 40 grad
				// 13 = 25 grad
				// 14 = 20 grad
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
	shortname += "s_";
	shortname += val;
	shortname += "_D";
	shortname += val_d;

	string filename;
	filename += "../results/";
	filename += shortname;
	filename += ".tex";

	saveToTex(filename, shortname, diagram);
}

void createDiagramQuadtreeCost(uint s, uint d) {
	char val[20];
	char val_d[20];
	sprintf(val_d, "%d", d);

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
				name += "/512x512_Delaunay/D";
				name += val_d;
				name += "/L";
				string sname = "\\begin{filecontents}{div_c_duration_info_";
				sname += val;
				sname += "_512x512D";
				sname += val_d;
				sname += "L";
		
				sprintf(val, "%.1f", l);
				name += val;
				name += "/I";
				sname += val;
				sname += "I";
				sprintf(val, "%.1f", i);
				name += val;
				name += "/_pre/results.tex";
				sname += val;
				sname += "pre.csv}";

				entry.z = loadFromTex(name, sname)[0];
				// 10 = 40 grad
				// 13 = 25 grad
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
	shortname += "s_";
	shortname += val;
	shortname += "_D";
	shortname += val_d;
	shortname += "_cost";

	string filename;
	filename += "../results/";
	filename += shortname;
	filename += ".tex";

	saveToTex(filename, shortname, diagram);
}


void createDiagramFloydSteinberg(uint s) {
	char val[20];

	Diagram diagram;
	for(double t = 0.0; t <= 1.0; t += 0.1) {
		for(double g = 0.0; g <= 1.0; g += 0.1) {
		
			Entry entry;
			entry.x = t;
			entry.y = g;
				
			sprintf(val, "%d", s);
			string name = "../results/";
			name += val;
			name += "/512x512_Delaunay/T";
			string sname = "\\begin{filecontents}{div_data_mean_";
			sname += val;
			sname += "_512x512T";
		
			sprintf(val, "%.1f", t);
			name += val;
			name += "/G";
			sname += val;
			sname += "G";
			sprintf(val, "%.1f", g);
			name += val;
			name += "/results.tex";
			sname += val;
			sname += ".csv}";

			entry.z = loadFromTex(name, sname)[14];
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
	shortname += "s_";
	shortname += val;
	shortname += "floydSteinberg";

	string filename;
	filename += "../results/";
	filename += shortname;
	filename += ".tex";

	saveToTex(filename, shortname, diagram);
}

void createDiagramFloydSteinbergCost(uint s) {
	char val[20];

	Diagram diagram;
	for(double t = 0.0; t <= 1.0; t += 0.1) {
		for(double g = 0.0; g <= 1.0; g += 0.1) {
		
			Entry entry;
			entry.x = t;
			entry.y = g;
				
			sprintf(val, "%d", s);
			string name = "../results/";
			name += val;
			name += "/512x512_Delaunay/T";
			string sname = "\\begin{filecontents}{div_c_duration_info_";
			sname += val;
			sname += "_512x512T";
		
			sprintf(val, "%.1f", t);
			name += val;
			name += "/G";
			sname += val;
			sname += "G";
			sprintf(val, "%.1f", g);
			name += val;
			name += "/results.tex";
			sname += val;
			sname += ".csv}";

			entry.z = loadFromTex(name, sname)[0];
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
	shortname += "s_";
	shortname += val;
	shortname += "floydSteinberg_cost";

	string filename;
	filename += "../results/";
	filename += shortname;
	filename += ".tex";

	saveToTex(filename, shortname, diagram);
}

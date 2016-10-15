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

struct Entry {
	double x;
	double y;
	double z;
};

typedef vector<Entry> 	Diagram;
typedef	vector<double> 	Col;
typedef	unsigned int	uint;

Col 	loadFromTex(string file);
void 	saveToTex(string file);

double	sumCol(const Col& col);

int main(int argc, char* argv[]) {
	loadFromTex("../results/4/512x512_Delaunay/D8/L0.2/I0.2/results.tex");

	for(double l = 0.0; l <= 1.0; l += 0.1) {
		for(double i = 0.0; i <= l; i += 0.1) {
			char val[20];

			string name = "../results/4/512x512_Delaunay/D8/L";
			
			sprintf(val, "%.1f", l);
			name += val;
			name += "/I";
			sprintf(val, "%.1f", i);
			name += val;
			name += "/results.tex";

			cout<<name<<endl;
		}
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
	while(getline(file, str_line, '\n')) {
		if(str_line == "a,p,r,g,b,m")
			break;
	}

	string value;
	while(getline(file, str_line, '\n')) {
		if(str_line == "\\end{filecontents}")
			break;

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
	for(uint i = 0; i < size; ++i)
		sum += col[i];

	return sum;
}

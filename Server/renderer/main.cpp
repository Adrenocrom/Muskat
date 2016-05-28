#include "muskat.h"


void init();

void render();

void exit();

int main(int argc, char* argv[]) {
	
	properties prop(argc, argv);
	prop.width  = 1024;
	prop.height = 768;
	muskatInit(prop, init);
	
	muskatRun(render);

	return 0;
}

void init() {

}

void render() {
	
}

void exit() {

}

#include "muskat.h"

void muskatInit(properties& prop, void (*init)()) {
	glutInit(&prop.argc, prop.argv);
	   
	glutInitContextVersion(4, 0);
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
	glutInitContextProfile(GLUT_CORE_PROFILE);

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
								  
	glutInitWindowSize(prop.width, prop.height);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	prop.handle = glutCreateWindow(prop.prefix.c_str());
	
	if(prop.handle < 1) {
	     cerr<<"ERROR: Could not create a new rendering window."<<endl;
		  exit(EXIT_FAILURE);
	}

	GLenum GlewInitResult;
   glewExperimental = GL_TRUE;
   GlewInitResult = glewInit();

	if (GLEW_OK != GlewInitResult) {
  		cerr<<"ERROR: "<<glewGetErrorString(GlewInitResult)<<endl;
    	exit(EXIT_FAILURE);
  	}
  
  	cout<<"INFO: OpenGL Version: "<<glGetString(GL_VERSION)<<endl;
  	glGetError();

	init();
}

void muskatRun(void (*render)()) {
	glutDisplayFunc(render);
	glutMainLoop();	
}


void muskatExit(void (*exit)()) {
	
}


#include "muskat.h"

mt_properties properties;

void muskatInit(void (*init)()) {
    glutInit(&properties.argc, properties.argv);
	   
	glutInitContextVersion(4, 0);
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
	glutInitContextProfile(GLUT_CORE_PROFILE);

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
								  
    glutInitWindowSize(properties.width, properties.height);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    properties.handle = glutCreateWindow(properties.prefix.c_str());
	
    if(properties.handle < 1) {
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
    glutIdleFunc(glutPostRedisplay);

    glViewport(0, 0, properties.width, properties.height);
	init();
}

void muskatRun(void (*render)(), void (*move)(int v)) {
	glutDisplayFunc(render);
    glutTimerFunc(0, move, 0);
    //glutMainLoop();
    glutMainLoopEvent();
}

void muskatResize(void (*resize)(int w, int h)) {
    glutReshapeFunc(resize);
}


void muskatExit(void (*exit)()) {
    glutCloseFunc(exit);
}

void muskatResizeWindow(int w, int h) {
    glutReshapeWindow(w, h);
}

#ifndef MUSKAT_H
#define MUSKAT_H

#pragma once

#include <iostream>
#include <cstring>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <turbojpeg.h>

#define MT_SAFE_DELETE(x)       	{if((x)) {delete (x); (x) = NULL;}}
#define MT_SAFE_DELETE_ARRAY(x) 	{if((x)) {delete[] (x); (x) = NULL;}}
#define MT_SAFE_RELEASE(x)      	{if((x)) {(x)->Release(); (x) = NULL;}}
#define MT_SAFE_MEMFREE(x)			{if((x)) {free((x)); (x) = NULL;}}

using namespace std;

struct mt_properties {
	int argc;
	char** argv;
    unsigned int width;
    unsigned int height;
	int handle;
	string prefix;
    unsigned int frameCnt;

    mt_properties() {
        argc = 0;
        argv = NULL;
		width 	= 800;
		height 	= 600;
        frameCnt= 0;
		handle 	= 0;
		prefix 	= "Muskat";
    }
};

extern mt_properties properties;

void muskatInit(void (*init)());
void muskatResize(void (*resize)(int w, int h));
void muskatRun(void (*render)(), void (*move)(int v));
void muskatExit(void (*exit)());

#include "utils.h"
#include "si.h"

#endif

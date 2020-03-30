#ifndef VISUALIZER_H
#define VISUALIZER_H

#include <ctime>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <math.h>
#include <vector>

// #include <OpenGL/gl.h>
// #include <OpenGl/glu.h>
// #include <GLUT/glut.h>
// clang-format off
#include <GL/glut.h>
#include <GLFW/glfw3.h>
#include <GL/gl.h>
// clang-format on

#include "visualizer/view_angles.h"
#include "visualizer/axes_box.h"

void releaseKey(int key, int x, int y);
void pressKey(int key, int xx, int yy);
void draw();

void processNormalKeys(unsigned char key, int x, int y);
void processNormalKeysRelease(unsigned char key, int x, int y);
void changeSize(int w, int h);
void renderScene(void);

class Visualizer
{
private:
public:
  Visualizer(int argc, char *argv[]);

  ViewAngles view_angles;
  AxesBox axes_box;
};

extern Visualizer *visualizer;

#endif

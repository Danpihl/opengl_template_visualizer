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

#include <OpenGL/gl.h>
#include <OpenGl/glu.h>
#include <GLUT/glut.h>

#include "sphere_subdivision/subdivisor.h"
#include "soft_ball/soft_ball.h"

void releaseKey(int key, int x, int y);
void pressKey(int key, int xx, int yy);
void draw();

void processNormalKeys(unsigned char key, int x, int y);
void processNormalKeysRelease(unsigned char key, int x, int y);
void changeSize(int w, int h);
void renderScene(void);
void computePos(float delta_move);
void computeDir(float delta_angle);

class Visualizer
{
  private:
  public:
    Visualizer(int argc, char *argv[], SoftBall *soft_ball_);

    SoftBall *soft_ball;
    int N;
    float angle;
    float lx;
    float lz;
    float x;
    float z;
    float delta_angle;
    float delta_move;

    float delta_angle_factor;
    float delta_move_factor;

    bool draw_spheres;
    bool draw_faces;
    bool draw_connections;
};

extern Visualizer *visualizer;

#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>

#include <vector>
#include <string>
#include <ctime>
#include <chrono>
#include <iostream>
#include <fstream>
#include <algorithm>

#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>

#include "soft_ball/soft_ball.h"
#include "misc/colors.h"
#include "visualizer/visualizer.h"
#include "movie_creator/movie_creator.h"

#include "sphere_subdivision/subdivisor.h"
#include "shading/shading.h"

Subdivisor *subdivisor;
SoftBall *soft_ball;

using namespace std;

int main(int argc, char *argv[])
{

    float c = 4.0f;         // Exterior dampening
    float cc = 4.0f;        // Center to exterior dampening
    float ca = 0.0f * 1.0f; // Air resistance
    float m = 0.05f;        // Exterior mass
    float mc = 1.0f;        // Center mass
    float x0 = 0.0f;
    float y0 = 3.0f;
    float z0 = 0.0f;
    float K = 10000.0f;  // Exterior spring constant
    float Kc = 10000.0f; // Center to exterior spring constant
    float h = 0.0001f;   // Time step
    float Lc = 3.0f;     // Length between center and exterior
    int num_subdivisions = 3;

    soft_ball = new SoftBall(m, c, K, h, Kc, cc, mc, Lc, ca, x0, y0, z0, num_subdivisions);
    visualizer = new Visualizer(argc, argv, soft_ball);
    glutMainLoop();

    // createMovie();

    return 0;
}

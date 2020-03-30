#ifndef SHADING_H
#define SHADING_H

#include <ctime>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <chrono>
#include <ctime>
#include <iostream>
#include <fstream>
#include <ios>
#include <unistd.h>

// clang-format off
#include <GL/glut.h>
#include <GLFW/glfw3.h>
#include <GL/gl.h>
// clang-format on

GLuint LoadShaders(const char *vertex_file_path, const char *fragment_file_path);

#endif
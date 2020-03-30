
#include "visualizer/visualizer.h"

int main(int argc, char *argv[])
{
    visualizer = new Visualizer(argc, argv);
    glutMainLoop();

    return 0;
}

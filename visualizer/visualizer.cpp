#include "visualizer/visualizer.h"

#include <algorithm>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/core.hpp>

#include <arl/math/math.h>
#include <arl/utilities/logging.h>

#include "misc/colors.h"
#include "visualizer/view_angles.h"

#define DELTA_MOVE_FACTOR 1.5f
#define DELTA_ANGLE_FACTOR 0.005f
#define world_edge 10.0f
#define GLOBAL_Y (-2.0f)

Visualizer *visualizer;

GLuint programID;
cv::Mat img;
bool is_recording;
int img_idx;

// #define WINDOW_WIDTH 1200
// #define WINDOW_HEIGHT 800
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800

arl::Vec3Dd axes_center(0.0, 0.0, 0.0);

const std::vector<std::string> mouse_strings = {"Left", "Middle", "Right"};

class MouseState
{
private:
public:
    int x;
    int y;
    int px;
    int py;
    int dx;
    int dy;
    const int button_enum;
    int is_pressed;

    MouseState(const int button_enum_) : button_enum(button_enum_)
    {
        is_pressed = 0;
        x = -1;
        y = -1;
        px = -1;
        py = -1;
        dx = 0;
        dy = 0;
    }

    void updateOnPress(const int button, const int state, const int x_, const int y_)
    {
        if (button == button_enum)
        {
            is_pressed = !state;
            if (is_pressed)
            {
                x = x_;
                y = y_;
                px = x_;
                py = y_;
                dx = 0;
                dy = 0;
            }
            else
            {
                x = -1;
                y = -1;
                px = -1;
                py = -1;
                dx = 0;
                dy = 0;
            }
        }
    }

    void updateOnMotion(int x_, int y_)
    {
        if (is_pressed)
        {
            px = x;
            py = y;
            x = x_;
            y = y_;
            dx = x - px;
            dy = y - py;
        }
    }

    void print() const
    {
        const std::string p = is_pressed ? "pressed" : "not pressed";
        std::cout << "Button " << mouse_strings[button_enum] << " is " << p
                  << " with xy coords [ " << x << ", " << y << " ]" << std::endl;
    }
};

MouseState left_mouse_button(GLFW_MOUSE_BUTTON_1);
MouseState scroll_mouse_button(GLFW_MOUSE_BUTTON_2);
MouseState right_mouse_button(GLFW_MOUSE_BUTTON_3);

void mouseFunc(int button, int state, int x, int y)
{
    left_mouse_button.updateOnPress(button, state, x, y);
    scroll_mouse_button.updateOnPress(button, state, x, y);
    right_mouse_button.updateOnPress(button, state, x, y);
}

void motionFunc(int x, int y)
{
    left_mouse_button.updateOnMotion(x, y);
    scroll_mouse_button.updateOnMotion(x, y);
    right_mouse_button.updateOnMotion(x, y);

    if (left_mouse_button.is_pressed)
    {
        const double mg = 0.001;
        visualizer->view_angles.changeAnglesWithDelta(-static_cast<double>(left_mouse_button.dx) * mg, -static_cast<double>(left_mouse_button.dy) * mg);
    }
    else if (scroll_mouse_button.is_pressed)
    {
        const double pg = 0.001;
        const double dx = scroll_mouse_button.dx;
        const double dy = scroll_mouse_button.dy;

        const arl::Matrixd rotation_mat = visualizer->view_angles.getRotationMatrix();
        const arl::Vec3Dd v = rotation_mat.inverse() * arl::Vec3Dd(-dx, dy, 0.0) * pg;

        axes_center = axes_center + v;
        visualizer->axes_box.moveCenter(v);
    }
    else if (right_mouse_button.is_pressed)
    {
        const double dy = right_mouse_button.dy;
        const double m_g = 0.001;
        const arl::Vec3Dd s = visualizer->axes_box.getScale();
        const arl::Vec3Dd inc_vec = arl::Vec3Dd(dy * s.x, dy * s.y, dy * s.z) * m_g;

        visualizer->axes_box.setMin(visualizer->axes_box.getMin() - inc_vec);
        visualizer->axes_box.setMax(visualizer->axes_box.getMax() + inc_vec);
    }
}

void addLighting()
{
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    GLfloat lightpos[] = {0.5, 3.0, 1.0, 0.};
    glLightfv(GL_LIGHT0, GL_POSITION, lightpos);
}

Visualizer::Visualizer(int argc, char *argv[])
{
    // TODO: Visualizer constructor should take draw function as argument

    is_recording = false;
    img_idx = 0;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(600, 200);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);

    glutCreateWindow("Opengl window");

    // register callbacks
    // glutDisplayFunc(renderScene);
    // glutReshapeFunc(changeSize);
    glutIdleFunc(renderScene);
    glutSpecialFunc(pressKey);
    glutIgnoreKeyRepeat(1);
    glutSpecialUpFunc(releaseKey);
    glutKeyboardFunc(processNormalKeys);
    glutKeyboardUpFunc(processNormalKeysRelease);

    glutMouseFunc(mouseFunc);
    glutMotionFunc(motionFunc);

    addLighting();

    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

    // img.create(WINDOW_HEIGHT, WINDOW_WIDTH, CV_8UC3);
    axes_box.setMin({-1.0, -1.0, -1.0});
    axes_box.setMax({1.0, 1.0, 1.0});

    glEnable(GL_DEPTH_TEST);
}

/*void changeSize(int w, int h)
{
    glutReshapeWindow(WINDOW_WIDTH, WINDOW_HEIGHT);
    // Prevent a divide by zero, when window is too short
    // (you cant make a window of zero width).
    if (h == 0)
    {
        h = 1;
    }
    float ratio = w * 1.0f / h;
    // Use the Projection Matrix
    // glMatrixMode(GL_PROJECTION);
    // Reset Matrix
    glLoadIdentity();
    // Set the viewport to be the entire window
    // glViewport(0, 0, w, h);
    // Set the correct perspective.
    // gluPerspective(45.0f, ratio, 0.1f, 100.0f);
    // Get Back to the Modelview
    // glMatrixMode(GL_MODELVIEW);
}*/

void processNormalKeysRelease(unsigned char key, int x, int y)
{
    (void)key;
    (void)x;
    (void)y;
}

void processNormalKeys(unsigned char key, int x, int y)
{
    (void)x;
    (void)y;
    if (key == 27)
    {
        exit(0);
    }
    else if (key == 'g')
    {
        is_recording = !is_recording;
    }
}

void renderScene(void)
{
    // Clear Color and Depth Buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix();

    const arl::AxisAngled ax_ang = visualizer->view_angles.getAngleAxis();

    glRotated(ax_ang.phi * 180.0f / M_PI, ax_ang.x, ax_ang.y, ax_ang.z);
    const arl::Vec3Dd s = visualizer->axes_box.getScale();
    glScaled(1.0 / s.x, 1.0 / s.y, 1.0 / s.z);

    const arl::Vec3Dd center_vec = visualizer->axes_box.getCenter();
    glTranslated(-center_vec.x, -center_vec.y, -center_vec.z);

    draw();
    glColor3f(0.1, 0.0, 1.0);
    glPopMatrix();

    glutSwapBuffers();
}

void pressKey(int key, int xx, int yy)
{
    (void)xx;
    (void)yy;
    switch (key)
    {
    case GLUT_KEY_LEFT:
        break;
    case GLUT_KEY_RIGHT:
        break;
    case GLUT_KEY_UP:
        break;
    case GLUT_KEY_DOWN:
        break;
    }
}

void releaseKey(int key, int x, int y)
{
    (void)x;
    (void)y;
    switch (key)
    {
    case GLUT_KEY_LEFT:
        break;
    case GLUT_KEY_RIGHT:
        break;
    case GLUT_KEY_UP:
        break;
    case GLUT_KEY_DOWN:
        break;
    default:

        break;
    }
}

void draw()
{
    glLineWidth(1.4f);

    const double max_val = 10.0;

    glutSolidSphere(0.1, 10, 10);
    glutWireSphere(0.1, 10, 10);

    glColor3d(0.0, 0.0, 0.0);

    glBegin(GL_LINES);
    glVertex3f(max_val, 0.0, 0.0);
    glVertex3f(-max_val, 0.0, 0.0);
    glEnd();

    glBegin(GL_LINES);
    glVertex3f(0.0, max_val, 0.0);
    glVertex3f(0.0, -max_val, 0.0);
    glEnd();

    glBegin(GL_LINES);
    glVertex3f(0.0, 0.0, max_val);
    glVertex3f(0.0, 0.0, -max_val);
    glEnd();

    glBegin(GL_TRIANGLES);
    glColor3f(0.5, 1.0, 0);
    glNormal3f(0.0, 1.0, 0.0);

    glVertex3d(0.0, 0.0, 0.0);
    glVertex3f(-1.0, 0.0, 2.0);
    glVertex3f(1.3, 0.0, 1.5);
    glEnd();
}

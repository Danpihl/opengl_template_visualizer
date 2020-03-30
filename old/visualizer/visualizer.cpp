#include "visualizer.h"
#include "shading/shading.h"
#include "sphere_subdivision/subdivisor.h"
#include "sphere_subdivision/face.h"
#include "sphere_subdivision/edge.h"
#include "misc/colors.h"

#include <algorithm>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/core.hpp>

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
#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800

Visualizer::Visualizer(int argc, char *argv[], SoftBall *soft_ball_) : draw_spheres(true),
                                                                           draw_faces(false),
                                                                           draw_connections(true)
{
    soft_ball = soft_ball_;

    N = soft_ball->N;

    angle = 0.0f;
    lx = 0.0f;
    lz = -1.0f;
    x = 0.0f;
    z = 13.0f;
    delta_angle_factor = 0.0f;
    delta_move_factor = 0.0f;
    delta_angle = 0.0f;
    delta_move = 0.0f;

    is_recording = false;
    img_idx = 0;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(600, 200);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);

    glutCreateWindow("Ball simulation");

    // register callbacks
    glutDisplayFunc(renderScene);
    glutReshapeFunc(changeSize);
    glutIdleFunc(renderScene);
    glutSpecialFunc(pressKey);
    glutIgnoreKeyRepeat(1);
    glutSpecialUpFunc(releaseKey);
    glutKeyboardFunc(processNormalKeys);
    glutKeyboardUpFunc(processNormalKeysRelease);

    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

    img.create(WINDOW_HEIGHT, WINDOW_WIDTH, CV_8UC3);
    glPixelStorei(GL_PACK_ALIGNMENT, (img.step & 3) ? 1 : 4);
    glPixelStorei(GL_PACK_ROW_LENGTH, img.step / img.elemSize());

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);

    // Light model parameters:
    // -------------------------------------------

    GLfloat lmKa[] = {0.0, 0.0, 0.0, 0.0};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmKa);

    glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, 1.0);
    glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, 0.0);

    // -------------------------------------------
    // Spotlight Attenuation

    GLfloat spot_direction[] = {1.0, -1.0, -1.0};
    GLint spot_exponent = 30;
    GLint spot_cutoff = 180;

    glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spot_direction);
    glLighti(GL_LIGHT0, GL_SPOT_EXPONENT, spot_exponent);
    glLighti(GL_LIGHT0, GL_SPOT_CUTOFF, spot_cutoff);

    GLfloat Kc = 1.0;
    GLfloat Kl = 0.0;
    GLfloat Kq = 0.0;

    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, Kc);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, Kl);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, Kq);

    // -------------------------------------------
    // Lighting parameters:

    GLfloat light_pos[] = {0.0f, 5.0f, 5.0f, 1.0f};
    GLfloat light_Ka[] = {0.5f, 0.5f, 0.5f, 1.0f};
    GLfloat light_Kd[] = {0.1f, 0.1f, 0.1f, 1.0f};
    GLfloat light_Ks[] = {1.0f, 1.0f, 1.0f, 1.0f};

    // glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
    // glLightfv(GL_LIGHT0, GL_AMBIENT, light_Ka);
    // glLightfv(GL_LIGHT0, GL_DIFFUSE, light_Kd);
    // glLightfv(GL_LIGHT0, GL_SPECULAR, light_Ks);

    // -------------------------------------------
    // Material parameters:

    GLfloat material_Ka[] = {0.5f, 0.0f, 0.0f, 1.0f};
    GLfloat material_Kd[] = {0.4f, 0.4f, 0.5f, 1.0f};
    GLfloat material_Ks[] = {0.8f, 0.8f, 0.0f, 1.0f};
    GLfloat material_Ke[] = {0.1f, 0.0f, 0.0f, 0.0f};
    GLfloat material_Se = 20.0f;

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, material_Ka);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, material_Kd);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, material_Ks);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, material_Ke);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, material_Se);

    //

    const char *vertex_path = "../shading/vert.glsl";
    const char *fragment_path = "../shading/frag.glsl";
    programID = LoadShaders(vertex_path, fragment_path);
    // glUseProgram(programID);

    GLenum err;

    if ((err = glGetError()) != GL_NO_ERROR)
    {
        std::cout << "OpenGL error: " << err << std::endl;
    }

    glEnable(GL_DEPTH_TEST);
}

void changeSize(int w, int h)
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
    glMatrixMode(GL_PROJECTION);
    // Reset Matrix
    glLoadIdentity();
    // Set the viewport to be the entire window
    glViewport(0, 0, w, h);
    // Set the correct perspective.
    gluPerspective(45.0f, ratio, 0.1f, 100.0f);
    // Get Back to the Modelview
    glMatrixMode(GL_MODELVIEW);
}

void processNormalKeysRelease(unsigned char key, int x, int y)
{
}

void processNormalKeys(unsigned char key, int x, int y)
{

    if (key == 27)
    {
        exit(0);
    }
    else if (key == 'j')
    {
        float v[3] = {0.0f, 1.0f, 0.0f};
        visualizer->soft_ball->set_velocity(v, 10.0f);
    }
    else if (key == 'w')
    {
        float v[3] = {1.0f, 1.0f, 1.0f};
        visualizer->soft_ball->set_velocity(v, 10.0f);
    }
    else if (key == 'a')
    {
        float v[3] = {-1.0f, 0.0f, 0.0f};
        visualizer->soft_ball->set_velocity(v, 10.0f);
    }
    else if (key == 's')
    {
        float v[3] = {1.0f, 1.0f, -1.0f};
        visualizer->soft_ball->set_velocity(v, 10.0f);
    }
    else if (key == 'd')
    {
        float v[3] = {1.0f, 0.0f, 0.0f};
        visualizer->soft_ball->set_velocity(v, 10.0f);
    }
    else if (key == 'o')
    {

        visualizer->soft_ball->Kc += 500.0f;
        std::cout << "Kc: " << visualizer->soft_ball->Kc << std::endl;
    }
    else if (key == 'l')
    {

        visualizer->soft_ball->Kc -= 500.0f;
        std::cout << "Kc: " << visualizer->soft_ball->Kc << std::endl;
    }
    else if (key == 'e')
    {
        visualizer->draw_spheres = !visualizer->draw_spheres;
    }
    else if (key == 'r')
    {
        visualizer->draw_faces = !visualizer->draw_faces;
    }
    else if (key == 't')
    {
        visualizer->draw_connections = !visualizer->draw_connections;
    }
    else if (key == 'g')
    {
        is_recording = !is_recording;
    }
}

void draw()
{

    for (int k = 0; k < 40; k++)
    {
        visualizer->soft_ball->update();
    }

    glColor3f(0.3f, 0.5f, 0.75f);

    if (visualizer->draw_faces)
    {
        for (int k = 0; k < visualizer->soft_ball->subdivisor->face_point_indices.size(); k++)
        {
            std::vector<int> index_vector = visualizer->soft_ball->subdivisor->face_point_indices[k];
            const float v11 = visualizer->soft_ball->P[index_vector[0]];
            const float v12 = visualizer->soft_ball->P[visualizer->N + index_vector[0]];
            const float v13 = visualizer->soft_ball->P[2 * visualizer->N + index_vector[0]];
            const float v21 = visualizer->soft_ball->P[index_vector[1]];
            const float v22 = visualizer->soft_ball->P[visualizer->N + index_vector[1]];
            const float v23 = visualizer->soft_ball->P[2 * visualizer->N + index_vector[1]];
            const float v31 = visualizer->soft_ball->P[index_vector[2]];
            const float v32 = visualizer->soft_ball->P[visualizer->N + index_vector[2]];
            const float v33 = visualizer->soft_ball->P[2 * visualizer->N + index_vector[2]];

            const float q1 = -(v21 - v11);
            const float q2 = -(v22 - v12);
            const float q3 = -(v23 - v13);

            const float w1 = v31 - v11;
            const float w2 = v32 - v12;
            const float w3 = v33 - v13;
            const float c1 = q2 * w3 - q3 * w2;
            const float c2 = q3 * w1 - q1 * w3;
            const float c3 = q1 * w2 - q2 * w1;

            glBegin(GL_TRIANGLES);

            glColor3f(0.5, 0, 0);

            glNormal3f(c1, c2, c3);
            glVertex3f(v11, v12 + GLOBAL_Y, v13);

            glNormal3f(c1, c2, c3);
            glVertex3f(v21, v22 + GLOBAL_Y, v23);

            glNormal3f(c1, c2, c3);
            glVertex3f(v31, v32 + GLOBAL_Y, v33);

            glEnd();
        }
    }

    if (visualizer->draw_spheres)
    {
        // Draw balls
        for (int k = 0; k < visualizer->N; k++)
        {
            glColor3f(0.4f, 0.1f, 0.2f);

            glPushMatrix();

            float v1 = visualizer->soft_ball->P[k];
            float v2 = visualizer->soft_ball->P[visualizer->N + k];
            float v3 = visualizer->soft_ball->P[2 * visualizer->N + k];

            glTranslatef(v1, v2 + GLOBAL_Y, v3);

            glutSolidSphere(0.05f, 30, 30);
            glPopMatrix();
        }

        glLineWidth(5.5);
        glColor3f(0.0f, 0.0f, 0.0f);
    }

    glLineWidth(0.4f);
    if (visualizer->draw_connections)
    {
        // Draw connections
        for (int k = 0; k < visualizer->soft_ball->subdivisor->edgelist_subdivided.edges.size(); k++)
        {
            const int idx1 = visualizer->soft_ball->subdivisor->edgelist_subdivided.edges[k].i1;
            const int idx2 = visualizer->soft_ball->subdivisor->edgelist_subdivided.edges[k].i2;

            const float v1 = visualizer->soft_ball->P[idx1];
            const float v2 = visualizer->soft_ball->P[visualizer->N + idx1];
            const float v3 = visualizer->soft_ball->P[2 * visualizer->N + idx1];

            const float q1 = visualizer->soft_ball->P[idx2];
            const float q2 = visualizer->soft_ball->P[visualizer->N + idx2];
            const float q3 = visualizer->soft_ball->P[2 * visualizer->N + idx2];
            glBegin(GL_LINES);

            glVertex3f(v1, v2 + GLOBAL_Y, v3);
            glVertex3f(q1, q2 + GLOBAL_Y, q3);

            glEnd();
        }
    }
}

void computeDir(float angle)
{
    visualizer->lx = sin(angle);
    visualizer->lz = -cos(angle);
}

void computePos(float delta_move)
{
    visualizer->x += delta_move * visualizer->lx * 0.1f;
    visualizer->z += delta_move * visualizer->lz * 0.1f;
    //std::cout << "[" << visualizer->x << ", " << visualizer->z << "]" << std::endl;
}

void renderScene(void)
{

    if (visualizer->delta_move_factor)
    {
        visualizer->delta_move = visualizer->delta_move * 0.01 + 1 * visualizer->delta_move_factor;
        computePos(visualizer->delta_move);
    }
    if (visualizer->delta_angle_factor)
    {
        visualizer->delta_angle = 0.01 * visualizer->delta_angle + 3 * visualizer->delta_angle_factor;

        visualizer->angle += visualizer->delta_angle;
        computeDir(visualizer->angle);
    }

    // Clear Color and Depth Buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Reset transformations
    glLoadIdentity();
    // Set the camera
    gluLookAt(visualizer->x, 1.0f, visualizer->z,
              visualizer->x + visualizer->lx, 1.0f, visualizer->z + visualizer->lz,
              0.0f, 1.0f, 0.0f);

    glLineWidth(0.4f);
    const int num_lines = 10.0f;
    const float inc = 2.0f * world_edge / (static_cast<float>(num_lines) + 1.0f);
    float x = -world_edge;
    for (int k = 0; k < num_lines + 2; k++)
    {
        glBegin(GL_LINES);
        glVertex3f(-world_edge, GLOBAL_Y, x);
        glVertex3f(world_edge, GLOBAL_Y, x);
        glEnd();
        glBegin(GL_LINES);
        glVertex3f(x, GLOBAL_Y, -world_edge);
        glVertex3f(x, GLOBAL_Y, world_edge);
        glEnd();
        x += inc;
    }

    glPushMatrix();

    draw();
    glPopMatrix();

    glutSwapBuffers();

    if (is_recording)
    {
        glReadPixels(0, 0, img.cols, img.rows, GL_BGR, GL_UNSIGNED_BYTE, img.data);
        cv::imwrite("../images/" + std::to_string(img_idx) + ".png", img);
        img_idx = img_idx + 1;
    }

    //int w_ = glutGet(GLUT_WINDOW_WIDTH);
    //int h_ = glutGet(GLUT_WINDOW_HEIGHT);

    //cout << "Width: " << w_ << ", height: " << h_ << endl;
}

void pressKey(int key, int xx, int yy)
{

    switch (key)
    {
    case GLUT_KEY_LEFT:
        visualizer->delta_angle_factor = -DELTA_ANGLE_FACTOR;
        break;
    case GLUT_KEY_RIGHT:
        visualizer->delta_angle_factor = DELTA_ANGLE_FACTOR;
        break;
    case GLUT_KEY_UP:
        visualizer->delta_move_factor = DELTA_MOVE_FACTOR;
        break;
    case GLUT_KEY_DOWN:
        visualizer->delta_move_factor = -DELTA_MOVE_FACTOR;
        break;
    }
}

void releaseKey(int key, int x, int y)
{

    switch (key)
    {
    case GLUT_KEY_LEFT:
    case GLUT_KEY_RIGHT:
        visualizer->delta_angle_factor = 0.0f;
        visualizer->delta_angle = 0.0f;
        break;
    case GLUT_KEY_UP:
    case GLUT_KEY_DOWN:
        visualizer->delta_move_factor = 0.0f;
        visualizer->delta_move = 0.0f;
        break;
    }
}

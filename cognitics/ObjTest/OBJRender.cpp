#include <ccl/ObjLog.h>
#include <ccl/LogStream.h>
#include <ccl/Timer.h>
#include "scenegraphobj/scenegraphobj.h"
//#include "ip/pngwrapper.h"
#include <scenegraph/ExtentsVisitor.h>

#include <scenegraph_gl/scenegraph_gl.h>
#include "OBJRender.h"
#include <GL/glut.h>

namespace
{
    ccl::ObjLog logger;

    double window_llx;
    double window_lly;
    double window_urx;
    double window_ury;
    double cursor_world_x;
    double cursor_world_y;
    int width;
    int height;
    bool aoi_changed = false;
    bool leftMouseDown = false;
    int leftMouseDownX = 0;
    int leftMouseDownY = 0;

    bool rightMouseDown = false;
    int rightMouseDownX = 0;
    int rightMouseDownY = 0;
    int mousex = 0;
    int mousey = 0;

    double llx = 0;
    double lly = 0;
    double urx = 0;
    double ury = 0;
    scenegraph::Scene *scene = NULL;
    scenegraph::SimpleGLRenderVisitor renderVisitor("");
    scenegraph::ExtentsVisitor extentsVisitor;
}

void setAOI(double llx, double lly, double urx, double  ury)
{
    window_llx = llx;
    window_lly = lly;
    window_urx = urx;
    window_ury = ury;
    aoi_changed = true;
}

bool renderInit(int argc, char **argv, scenegraph::Scene *_scene)
{
    scene = _scene;
    logger.init("OBJ Render");
    logger << ccl::LINFO;
    // init GLUT and create window
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(50, 50);
    glutInitWindowSize(1000, 800);
    glutCreateWindow("OBJ Viewer");

    // register callbacks
    glutDisplayFunc(renderScene);
    glutReshapeFunc(changeSize);
    glutIdleFunc(renderScene);

    glutIgnoreKeyRepeat(1);
    glutKeyboardFunc(processNormalKeys);
    glutSpecialFunc(pressKey);
    glutSpecialUpFunc(releaseKey);

    // here are the two new functions
    glutMouseFunc(mouseButton);
    glutMotionFunc(mouseMove);
    glutPassiveMotionFunc(mouseMove);

    // OpenGL init
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    renderVisitor.setExtensions();
    double top = -DBL_MAX;
    double bottom = DBL_MAX;
    double left = DBL_MAX;
    double right = -DBL_MAX;
    double minZ = DBL_MAX;
    double maxZ = -DBL_MAX;
    extentsVisitor.visit(scene);
    extentsVisitor.getExtents(left, right, bottom, top, minZ, maxZ);

    double centerx = (left + right) / 2;
    double centery = (top + bottom) / 2;
    setAOI(centerx - 100, centery - 100, centerx + 100, centery + 100);

    // enter GLUT event processing cycle
    glutMainLoop();

    return true;
}

void renderScene(void)
{
    // Clear Color and Depth Buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //glClearColor(1.0f, 1.0f, 0.0f, 1.0f); //Chromakey background
    // Reset transformations
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glPushMatrix();
    glLoadIdentity();
    //gluOrtho2D(0, width, 0, height);
    glOrtho(0, width, 0, height, -5000, 5000);

    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2i(10, 10);
    std::stringstream ss;
    ss.precision(10);
    ss << "x=" << cursor_world_x << " y=" << cursor_world_y;
    RenderBitmapText(ss.str());
    ss.str() = "";

    glPopMatrix();

    // Set the camera
    //gluOrtho2D(window_llx, window_urx, window_lly, window_ury);
    glOrtho(window_llx, window_urx, window_lly, window_ury, -5000, 5000);
    //glDisable(GL_DEPTH_TEST);
    glMatrixMode(GL_MODELVIEW);

    // Draw 
    //
    //glEnable(GL_MULTISAMPLE);
    //glHint(GL_NICEST)
    glEnable(GL_BLEND);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glLineWidth(2.0);

    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glBegin(GL_LINE_LOOP);
    const float rad = 20.0;
    const float segments = 10;
    for (int i = 0; i < segments; i++) {
        float angle = i * 2 * 3.1459 / segments;
        double x = (cursor_world_x) + (cos(angle) * rad);
        double y = (cursor_world_y) + (sin(angle) * rad);
        glVertex2f(x, y);
    }
    glEnd();

    glPushMatrix();
    renderVisitor.visit(scene);
    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glDisable(GL_BLEND);
    //glDisable(GL_LINE_SMOOTH);
    glPopMatrix();
    glutSwapBuffers();
}

void processNormalKeys(unsigned char key, int xx, int yy)
{

    //glutSetMenu(mainMenu);
    switch (key) {
    case 27:
        //glutDestroyMenu(mainMenu);
        //glutDestroyMenu(fillMenu);
        //glutDestroyMenu(colorMenu);
        //glutDestroyMenu(shrinkMenu);
        exit(0);
        break;

    case ' ':
    {
        // Reset view				
        double centerx = (llx + urx) / 2;
        double centery = (lly + ury) / 2;
        setAOI(centerx - 5000, centery - 5000, centerx + 5000, centery + 5000);
    }
    break;
    }
    if (key == 27)
        exit(0);
}

void pressKey(int key, int xx, int yy)
{
    /*
        switch (key) {
            case GLUT_KEY_UP : deltaMove = 0.5f; break;
            case GLUT_KEY_DOWN : deltaMove = -0.5f; break;
        }
        */
}

void releaseKey(int key, int x, int y)
{
    /*
    switch (key) {
        case GLUT_KEY_UP :
        case GLUT_KEY_DOWN : break;
    }
    */
}

void changeSize(int w, int h)
{

    // Prevent a divide by zero, when window is too short
    // (you cant make a window of zero width).
    if (h == 0)
        h = 1;

    float ratio = w * 1.0 / h;
    height = h;
    width = w;
    // Set the viewport to be the entire window
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // Set the camera
    gluOrtho2D(window_llx, window_urx, window_lly, window_ury);
    glMatrixMode(GL_MODELVIEW);

}



void mouseMove(int x, int y)
{
    mousex = x;
    mousey = y;
    double width_world = window_urx - window_llx;
    double height_world = window_ury - window_lly;

    cursor_world_x = window_llx + ((double(x) / double(width))*width_world);
    cursor_world_y = window_ury - ((double(y) / double(height))*height_world);

    if (leftMouseDown)
    {
        double mouseDeltaX = leftMouseDownX - x;
        double mouseDeltaY = leftMouseDownY - y;

        mouseDeltaX *= (width_world / width);
        mouseDeltaY *= (height_world / height);

        setAOI(window_llx + mouseDeltaX, window_lly - mouseDeltaY, window_urx + mouseDeltaX, window_ury - mouseDeltaY);
        leftMouseDownX = x;
        leftMouseDownY = y;
    }
    if (rightMouseDown)
    {
        double mouseDeltaX = rightMouseDownX - x;
        double mouseDeltaY = rightMouseDownY - y;
        double mdist = sqrt(double((mouseDeltaX*mouseDeltaX) + (mouseDeltaY*mouseDeltaY)));
        if (mouseDeltaY < 0)
            mdist *= -1;


        double centerx = (window_llx + window_urx) / 2;
        double centery = (window_lly + window_ury) / 2;
        double factor = (mdist / 10) + 1;

        //printf("f=%f\n",factor);
        width_world *= factor;
        height_world *= factor;

        setAOI(centerx - (width_world / 2), centery - (height_world / 2), centerx + (width_world / 2), centery + (height_world / 2));

        rightMouseDownX = x;
        rightMouseDownY = y;
    }
}

void mouseButton(int button, int state, int x, int y)
{
    double width_world = window_urx - window_llx;
    double height_world = window_ury - window_lly;
    // only start motion if the left button is pressed
    if (button == GLUT_LEFT_BUTTON)
    {
        // when the button is released
        if (state == GLUT_UP)
        {
            leftMouseDown = false;
        }
        else
        {// state = GLUT_DOWN
            leftMouseDown = true;
            leftMouseDownX = x;
            leftMouseDownY = y;
        }
    }
    if (button == GLUT_RIGHT_BUTTON)
    {
        // when the button is released
        if (state == GLUT_UP)
        {
            rightMouseDown = false;
        }
        else
        {// state = GLUT_DOWN

            rightMouseDown = true;
            rightMouseDownX = x;
            rightMouseDownY = y;
        }
    }
    if (button == 4) // scroll up
    {
        if (state == GLUT_UP)
        {
            double centerx = (window_llx + window_urx) / 2;
            double centery = (window_lly + window_ury) / 2;
            double factor = 1.1;
            width_world *= factor;
            height_world *= factor;
            setAOI(centerx - (width_world / 2), centery - (height_world / 2), centerx + (width_world / 2), centery + (height_world / 2));
        }
    }
    if (button == 3) // scroll down
    {
        if (state == GLUT_UP)
        {
            double centerx = (window_llx + window_urx) / 2;
            double centery = (window_lly + window_ury) / 2;
            double factor = 0.90;
            width_world *= factor;
            height_world *= factor;
            setAOI(centerx - (width_world / 2), centery - (height_world / 2), centerx + (width_world / 2), centery + (height_world / 2));
        }
    }
}

void RenderBitmapText(std::string str)
{
    BOOST_FOREACH(char c, str)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
    }
}
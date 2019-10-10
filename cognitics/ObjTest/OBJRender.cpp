#include <ccl/ObjLog.h>
#include <ccl/LogStream.h>
#include <ccl/Timer.h>
#include "scenegraphobj/scenegraphobj.h"
//#include "ip/pngwrapper.h"
#include <scenegraph/ExtentsVisitor.h>

#include <GL/glew.h>
#include <GL/glut.h>
#include <scenegraph_gl/scenegraph_gl.h>
#include "OBJRender.h"
#include "ip/pngwrapper.h"

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
    scenegraph::Scene *fixedScene = NULL;
    renderJobList_t renderJobs;
}

void setAOI(double llx, double lly, double urx, double  ury)
{
    window_llx = llx;
    window_lly = lly;
    window_urx = urx;
    window_ury = ury;
    aoi_changed = true;
}

void renderToFile(RenderJob &job)
{
    int width = 1024;
    int height = 1024;
    int depth = 3;

    scenegraph::Scene *scene = new scenegraph::Scene();
    for (auto&&obj : job.objFiles)
    {
        scenegraph::Scene *childScene = scenegraph::buildSceneFromOBJ(obj, true);
        scene->addChild(childScene);
    }

    // Build the texture that will serve as the depth attachment for the framebuffer.
    GLuint depth_texture;
    glGenTextures(1, &depth_texture);
    glBindTexture(GL_TEXTURE_2D, depth_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);

    // The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
    GLuint FramebufferName = 0;
    glGenFramebuffers(1, &FramebufferName);
    glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
    // The texture we're going to render to
    GLuint renderedTexture;
    glGenTextures(1, &renderedTexture);

    // "Bind" the newly created texture : all future texture functions will modify this texture
    glBindTexture(GL_TEXTURE_2D, renderedTexture);

    // Give an empty image to OpenGL ( the last "0" )
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

    // Poor filtering. Needed !
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // Set "renderedTexture" as our colour attachement #0
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0);

    // Set the list of draw buffers.
    GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

    // Always check that our framebuffer is ok
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        logger << "Frame Buffer Error!" << logger.endl;
        return;
    }

    //GLuint ProgramID = LoadShaders("", "layout(location = 0) out vec3 color;");

    // Render to our framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_texture, 0);

    glViewport(0, 0, width, height); // Render on the whole framebuffer, complete from the lower left corner to the upper right
    glEnable(GL_DEPTH_TEST);
    // Use our shader
    //glUseProgram(ProgramID);

        // Clear Color and Depth Buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //glClearColor(1.0f, 1.0f, 0.0f, 1.0f); //Chromakey background
    // Reset transformations
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glPushMatrix();
    glLoadIdentity();

    glOrtho(0, width, 0, height, -5000, 5000);

    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2i(10, 10);
    

    glPopMatrix();

    glOrtho(window_llx, window_urx, window_lly, window_ury, -5000, 5000);
    glMatrixMode(GL_MODELVIEW);

    // Draw
    glEnable(GL_BLEND);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glPushMatrix();
    renderVisitor.visit(scene);
    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glDisable(GL_BLEND);
    glPopMatrix();
    glutSwapBuffers();

    unsigned char *pixels = new unsigned char[width * height * depth];
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);
    ip::ImageInfo info;
    info.width = width;
    info.height = height;
    info.depth = depth;
    info.interleaved = true;
    info.dataType = ip::ImageInfo::UBYTE;
    ccl::binary buf;
    for (int i = 0; i < (width * height * depth); i++)
        buf.push_back(pixels[i]);
    ip::FlipVertically(info, buf);
    std::string pngName = job.cdbFilename + ".png";
    logger << "Writing " << pngName << logger.endl;
    ip::WritePNG24(pngName, info, buf);
    delete pixels;
    
    double top = -DBL_MAX;
    double bottom = DBL_MAX;
    double left = DBL_MAX;
    double right = -DBL_MAX;
    double minZ = DBL_MAX;
    double maxZ = -DBL_MAX;
    extentsVisitor = scenegraph::ExtentsVisitor();
    extentsVisitor.visit(scene);
    extentsVisitor.getExtents(left, right, bottom, top, minZ, maxZ);
    setAOI(left, bottom, right, top);
    delete scene;
}

scenegraph::Scene *getCurrentScene()
{
#if 0
    if (scene)
        return scene;
    /*
    if (renderFiles.size() <= currentFileNum)
    {
        if (!scene)
        {
            scene = scenegraph::buildSceneFromOBJ(renderFiles[currentFileNum].getFileName(), true);
        }
        return scene;
    }
    */
    if (fixedScene)
        scene = fixedScene;
    else
        scene = scenegraph::buildSceneFromOBJ(renderFiles[currentFileNum].getFileName(), true);
    double top = -DBL_MAX;
    double bottom = DBL_MAX;
    double left = DBL_MAX;
    double right = -DBL_MAX;
    double minZ = DBL_MAX;
    double maxZ = -DBL_MAX;
    extentsVisitor = scenegraph::ExtentsVisitor();
    extentsVisitor.visit(scene);
    extentsVisitor.getExtents(left, right, bottom, top, minZ, maxZ);
    setAOI(left, bottom, right, top);
    
    return scene;
#endif
    return NULL;
}

#if 0

scenegraph::Scene *scene = scenegraph::buildSceneFromOBJ(fi.getFileName(), true);
double top = -DBL_MAX;
double bottom = DBL_MAX;
double left = DBL_MAX;
double right = -DBL_MAX;
double minZ = DBL_MAX;
double maxZ = -DBL_MAX;
extentsVisitor.visit(scene);
extentsVisitor.getExtents(left, right, bottom, top, minZ, maxZ);

#endif

bool renderingToFile = false;


bool renderInit(int argc, char **argv, renderJobList_t &jobs)//std::vector<ccl::FileInfo> files, scenegraph::Scene *_fixedScene)
{
    renderJobs = jobs;
    //fixedScene = _fixedScene;
    //glutInitDisplayMode(GLUT_RGB);
    //renderFiles = files;
    //scene = _scene;
    logger.init("OBJ Render");
    logger << ccl::LINFO;
    // init GLUT and create window
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    
    glutInitWindowPosition(50, 50);
    glutInitWindowSize(1024, 1024);
    glutCreateWindow("OBJ Viewer");

    glewExperimental = GL_TRUE;
    glewInit();

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

    // enter GLUT event processing cycle
    glutMainLoop();

    return true;
}

void renderScene(void)
{
    if(renderJobs.empty())
    {
        exit(0);
    }
    RenderJob job = renderJobs.back();
    renderJobs.pop_back();
    
    renderToFile(job);

    //scene = getCurrentScene();
    /*
    if (!scene)
        exit(0);
    if (renderingToFile)
    {
        renderToFile();
    }
    */
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
    if (!renderingToFile)
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

    glPushMatrix();
    renderVisitor.visit(scene);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glDisable(GL_BLEND);
    //glDisable(GL_LINE_SMOOTH);
    glPopMatrix();
    glutSwapBuffers();
}

GLuint LoadShaders(std::string vertexShader, std::string fragmentShader)
{

    // Create the shaders
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    GLint Result = GL_FALSE;
    int InfoLogLength;

    if (vertexShader.size() > 0)
    {
        // Compile Vertex Shader
        logger << "Compiling vertex shader..." << logger.endl;
        char const * VertexSourcePointer = vertexShader.c_str();
        glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
        glCompileShader(VertexShaderID);

        // Check Vertex Shader
        glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
        glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
        if (InfoLogLength > 0) {
            std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
            glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
            logger << (&VertexShaderErrorMessage[0]) << logger.endl;
        }
    }

    if (fragmentShader.size() > 0)
    {
        // Compile Fragment Shader
        logger << "Compiling fragment shader..." << logger.endl;
        char const * FragmentSourcePointer = fragmentShader.c_str();
        glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
        glCompileShader(FragmentShaderID);

        // Check Fragment Shader
        glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
        glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
        if (InfoLogLength > 0) {
            std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
            glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
            logger << (&FragmentShaderErrorMessage[0]) << logger.endl;
        }
    }
    // Link the program
    logger << "Linking program..." << logger.endl;
    GLuint ProgramID = glCreateProgram();
    
    if (vertexShader.size() > 0)
        glAttachShader(ProgramID, VertexShaderID);
    if (fragmentShader.size() > 0)
        glAttachShader(ProgramID, FragmentShaderID);

    glLinkProgram(ProgramID);

    // Check the program
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
        glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
        logger << (&ProgramErrorMessage[0]) << logger.endl;
    }

    if (vertexShader.size() > 0)
    {
        glDetachShader(ProgramID, VertexShaderID);
        glDeleteShader(VertexShaderID);
    }
    if (fragmentShader.size() > 0)
    {
        glDetachShader(ProgramID, FragmentShaderID);
        glDeleteShader(FragmentShaderID);

    }

    return ProgramID;
}




void processNormalKeys(unsigned char key, int xx, int yy)
{

    //glutSetMenu(mainMenu);
    switch (key) {
    case 27:
        exit(0);
        break;
    case 'r':
    {
        renderingToFile = true;

        
        break;
    }
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
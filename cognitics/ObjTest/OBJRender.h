#pragma once

#include <string>
#include <vector>

void setAOI(double llx, double lly, double urx, double  ury);

bool renderInit(int argc, char **argv, std::vector<ccl::FileInfo> objFiles);
void pressKey(int key, int xx, int yy);
void releaseKey(int key, int x, int y);
void renderScene(void);
void changeSize(int w, int h);
void processNormalKeys(unsigned char key, int xx, int yy);
void mouseMove(int x, int y);
void mouseButton(int button, int state, int x, int y);
void RenderBitmapText(std::string str);
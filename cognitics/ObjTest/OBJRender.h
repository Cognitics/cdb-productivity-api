#pragma once

#include <string>
#include <vector>
#include "cdb_tile/Tile.h"

class RenderJob
{
public:
    std::string cdbFilename;
    double enuMinX;
    double enuMinY;
    double enuMaxX;
    double enuMaxY;
    cognitics::cdb::Tile cdbTile;
    std::vector<std::string> objFiles;

    RenderJob(const cognitics::cdb::Tile tile) : cdbTile(tile)
    {
        enuMinX = 0;
        enuMinY = 0;
        enuMaxX = 0;
        enuMaxY = 0;
    }
};

typedef std::list<RenderJob> renderJobList_t;

void setAOI(double llx, double lly, double urx, double  ury);

bool renderInit(int argc, char **argv, renderJobList_t &jobs) ;
void pressKey(int key, int xx, int yy);
void releaseKey(int key, int x, int y);
void renderScene(void);
void changeSize(int w, int h);
void processNormalKeys(unsigned char key, int xx, int yy);
void mouseMove(int x, int y);
void mouseButton(int button, int state, int x, int y);
void RenderBitmapText(std::string str);
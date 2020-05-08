#pragma once

#include "mesh2cdb.h"
typedef std::list<RenderJob> renderJobList_t;

void setAOI(double llx, double lly, double urx, double  ury);

bool renderInit(int argc, char **argv, renderJobList_t &jobs, const std::string &cdbRoot);
bool renderScene(void);


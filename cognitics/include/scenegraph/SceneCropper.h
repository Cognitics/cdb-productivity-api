/****************************************************************************
Copyright (c) 2019 Cognitics, Inc.

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
****************************************************************************/
#pragma once

#include "Scene.h"

namespace scenegraph {

    //!    Crops all Faces in a Scene to the bounding box specified by [xmin, ymin] and [xmax, ymax].
    //!    This does not alter the original Scene, but instead creates and returns a new Scene.
    Scene* cropScene(const Scene* scene, double xmin, double xmax, double ymin, double ymax, const sfa::Matrix &m = sfa::Matrix());

    //!    Bisect a Scene. Removing everything to the left of the line p1->p2 (in 2D space)
    Scene* cropScene(const Scene* scene, const sfa::Point& p1, const sfa::Point& p2);
}
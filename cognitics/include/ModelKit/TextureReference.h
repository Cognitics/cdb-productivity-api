/*************************************************************************
Copyright (c) 2020 Cognitics, Inc.

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

#include "ModelKit/Reference.h"
#include <sfa/Point.h>
#include <vector>

namespace modelkit
{
	typedef std::vector<sfa::Point> PointList;
	typedef Reference Texture;

	class TextureReference
	{
	private:
		Texture *texture;
		PointList UVs;

	public:
		~TextureReference(void);
		TextureReference(void);
		TextureReference(const TextureReference &copy);
		TextureReference(Texture *texture, const sfa::Point &a, const sfa::Point &b, const sfa::Point &c);

		void setTexture(Texture *texture);
		Texture *getTexture(void);

		void addUV(const sfa::Point &uv);
		PointList& getUVs(void);
		sfa::Point &getUV(int index);
		void removeUV(int index);
		void clearUVs(void);

	};

}
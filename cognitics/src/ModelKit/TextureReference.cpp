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

#include "ModelKit/TextureReference.h"

namespace modelkit
{
	TextureReference::~TextureReference(void)
	{
	}

	TextureReference::TextureReference(void) : texture(NULL)
	{
	}

	TextureReference::TextureReference(const TextureReference &copy)
	{
		texture = copy.texture;
		UVs = copy.UVs;
	}

	TextureReference::TextureReference(Texture *texture, const sfa::Point &a, const sfa::Point &b, const sfa::Point &c) : texture(texture)
	{
		UVs.push_back(a);
		UVs.push_back(b);
		UVs.push_back(c);
	}

	void TextureReference::setTexture(Texture *texture)
	{
		this->texture = texture;
	}

	Texture *TextureReference::getTexture(void)
	{
		return texture;
	}

	void TextureReference::addUV(const sfa::Point &uv)
	{
		UVs.push_back(uv);
	}

	PointList& TextureReference::getUVs(void)
	{
		return UVs;
	}

	sfa::Point &TextureReference::getUV(int index)
	{
		return UVs[index];
	}

	void TextureReference::removeUV(int index)
	{
		UVs.erase(UVs.begin() + index);
	}

	void TextureReference::clearUVs(void)
	{
		UVs.clear();
	}


}
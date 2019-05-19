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

/*! \file ogr/Feature.h
\headerfile ogr/Feature.h
\brief Provides ogr::Layer.
\author Kevin Bentley <kbentley@cognitics.net>
\date 27 August 2012
*/
#pragma once

#include "ogr/Field.h"

#pragma warning ( push )
#pragma warning ( disable : 4251 )		// C4251: 'GDALColorTable::aoEntries' : class 'std::vector<_Ty>' needs to have dll-interface to be used by clients of class 'GDALColorTable'
#include <ogrsf_frmts.h>
#include <ogr_spatialref.h>
#pragma warning ( pop )

#include <sfa/sfa.h>

namespace ogr
{
/*! \brief OGR Feature Class

This class specializes sfa::Feature to allow I/O with OGR
*/
	class Layer;
	class Feature : public sfa::Feature
	{
	private:
		

	public:
		OGRFeature *ogr_native;
		Layer *layer;

		Feature() : ogr_native(NULL),layer(NULL)
		{

		}

		Feature(OGRFeature *_ogr_native,Layer *_layer) : ogr_native(_ogr_native),layer(_layer)
		{
			
		}

		virtual ~Feature(void)
		{
			if(ogr_native)
				OGRFeature::DestroyFeature(ogr_native);
		}

		int getOGRFID()
		{
			if(ogr_native)
			{
				return ogr_native->GetFID();
			}
			return -1;
		}
	};

}
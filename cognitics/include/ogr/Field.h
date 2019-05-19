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

/*! \file ogr/Field.h
\headerfile ogr/Field.h
\brief Provides ogr::Field.
\author Aaron Brinton <abrinton@cognitics.net>
\date 30 October 2009
*/
#pragma once

#pragma warning ( push )
#pragma warning ( disable : 4251 )		// C4251: 'GDALColorTable::aoEntries' : class 'std::vector<_Ty>' needs to have dll-interface to be used by clients of class 'GDALColorTable'
#include <ogrsf_frmts.h>
#pragma warning ( pop )

#include <string>
#include <vector>
#include <map>

namespace ogr
{
/*! \brief OGR Field Class

This class encapsulates the functionality of OGRFieldDefn.
*/
	class Field
	{
		friend class Layer;

	private:
		OGRFieldDefn *field;

		Field(OGRFieldDefn *field);

	public:
		~Field(void);

		OGRFieldDefn *getOGRFieldDefn(void);

		std::string getName(void) const;
		void setName(const std::string &name);

		OGRFieldType getType(void) const;
		std::string getTypeName(void);

	};

	typedef std::vector<Field *> FieldList;

}

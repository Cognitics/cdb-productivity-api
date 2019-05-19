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

#include "ogr/Field.h"

namespace ogr
{
	Field::~Field(void)
	{
	}

	Field::Field(OGRFieldDefn *field) : field(field)
	{
	}

	OGRFieldDefn *Field::getOGRFieldDefn(void)
	{
		return field;
	}

	std::string Field::getName(void) const
	{
		return std::string(field->GetNameRef());
	}

	void Field::setName(const std::string &name)
	{
		field->SetName(name.c_str());
	}

	OGRFieldType Field::getType(void) const
	{
		return field->GetType();
	}

	std::string Field::getTypeName(void)
	{
		switch(field->GetType())
		{
			case OFTInteger:		return "Integer";
			case OFTIntegerList:	return "IntegerList";
			case OFTReal:			return "Real";
			case OFTRealList:		return "RealList";
			case OFTString:			return "String";
			case OFTStringList:		return "StringList";
			case OFTWideString:		return "WideString";
			case OFTWideStringList:	return "WideStringList";
			case OFTBinary:			return "Binary";
			case OFTDate:			return "Date";
			case OFTTime:			return "Time";
			case OFTDateTime:		return "DateTime";
			default:				return "String";
		}
	}

}

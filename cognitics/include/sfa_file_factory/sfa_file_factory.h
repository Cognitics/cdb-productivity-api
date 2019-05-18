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
/*! \brief Provides sfa::FileFactory
\author Kevin Bentley <kbentley@cognitics.net>
\date 14 September 2012
*/
#pragma once
#include <vector>
#include <map>
#include <string>

#include <sfa/File.h>
namespace sfa
{
	//! Abstract interface definition for a sfa::File factory
	class IFileFactory
	{
	public:
		//! Create a new sfa::File instance
		virtual sfa::File *create() = 0;

		//! Destroy the specified sfa::File instance
		virtual void destroy(sfa::File *file) = 0;
	
	};
	
	//! Singleton sfa::File registry
	class FileRegistry
	{
		static FileRegistry *theInstance;
		std::map<std::string,IFileFactory *> registry;
		std::map<sfa::File *,IFileFactory *> ownerMap;
	public:
		//! Get the singleton FileRegistry instance
		static FileRegistry *instance();
		
		//! Register the sfa::File factory for the specified extension
		void registerFactory(std::string extention,IFileFactory *factory);

		//! Unregister the specified sfa::File factory
		void unRegisterFactory(IFileFactory *factory);
		
		//! Get a sfa::File instance for the specified filename
		sfa::File *getFile(std::string filename);

		//! Destroy the specified sfa::File instance
		void destroyFile(sfa::File *file);
	
	};

}


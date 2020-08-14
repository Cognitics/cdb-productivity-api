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
//#pragma optimize("", off)
#pragma warning ( disable : 4996 )        // deprecated access()

#include <ccl/ObjLog.h>
#include <ccl/LogStream.h>

#include "ccl/FileInfo.h"
#include "ccl/StringUtils.h"
#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <fstream>
#ifdef WIN32
#include <io.h>
#include <direct.h>
#include <Windows.h>
#else
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>

#endif

namespace ccl
{
    FileInfo::~FileInfo(void)
    {
    }

    FileInfo::FileInfo(const std::string &filename) : filename(filename)
    {
        for(size_t i=0;i<filename.size();i++)
            if(filename[i]=='\\') 
                this->filename[i]= '/';
    }

    FileInfo::FileInfo(const std::string &basePath, const std::string &filepart) 
    {
        filename = joinPaths(basePath,filepart);
        filename = standardizeSlashes(filename);        
    }

    std::string FileInfo::getFileName() const
    {
        return filename;
    }

    std::string FileInfo::getDirName(void)
    {
        std::string filecopy = standardizeSlashes(filename);
        std::string::size_type pos = filecopy.rfind('/');    
        return (pos == std::string::npos) ? "" : filename.substr(0, pos);
    }

    std::string FileInfo::getBaseName(bool withoutSuffix) const
    {
        std::string::size_type pos = filename.rfind('/');
        if(pos == std::string::npos)
            pos = filename.rfind('\\');
        std::string basename = (pos == std::string::npos) ? filename : filename.substr(pos + 1);
        if(!withoutSuffix)
            return basename;
        pos = basename.rfind('.');
        return (pos == std::string::npos) ? basename : basename.substr(0, pos);
    }

    std::string FileInfo::getSuffix(void)
    {
        std::string basename = getBaseName();
        std::string::size_type pos = basename.rfind('.');
        return (pos == std::string::npos) ? "" : basename.substr(pos + 1);
    }

    std::vector<FileInfo> FileInfo::getAllFiles(const std::string &basepath, const std::string &searchpattern, bool subdir_search)
    {
        std::vector<FileInfo> ret;
        std::vector<std::string> directory_frontier;
        directory_frontier.push_back(basepath);

        
        while (directory_frontier.size() > 0)
        {
            std::string basedir = directory_frontier.back();
            directory_frontier.pop_back();
            FileInfo searchPath(basedir, searchpattern);
#ifdef WIN32
            struct _finddata_t c_file;
            intptr_t hFile;

            if ((hFile = _findfirst(searchPath.getFileName().c_str(), &c_file)) == -1L)
            {
                return ret;
            }
            do
            {
                if (c_file.attrib & _A_SUBDIR)
                {
                    if (subdir_search)
                    {
                        if (strcmpi(c_file.name, "..") != 0 &&
                            strcmpi(c_file.name, ".") != 0)
                        {
                            directory_frontier.push_back(ccl::joinPaths(basedir, c_file.name));
                        }
                    }
                }
                else
                {
                    FileInfo file(basedir, c_file.name);
                    ret.push_back(file);
                }
            }while(_findnext( hFile, &c_file ) == 0 );
            _findclose(hFile);
#else
            DIR *dp = opendir(basedir.c_str());
            if(dp)
            {
                dirent *dirp;
                while((dirp = readdir(dp)) != NULL)
                {
                  if(dirp->d_type==DT_DIR && subdir_search)
                    {
                                      if(strcmp(dirp->d_name,"..")!=0 && strcmp(dirp->d_name,".")!=0)
                                        {
                                          directory_frontier.push_back(ccl::joinPaths(basedir, dirp->d_name));
                                        }
                    }
                  else
                    {
                        if(strcmp(dirp->d_name,"..")!=0 && strcmp(dirp->d_name,".")!=0)
                        {
                            FileInfo file(basedir,dirp->d_name);
                            ret.push_back(file);            
                        }
                    }
                }
                closedir(dp);
            }
#endif
        }
        return ret;
    }


    std::vector<std::string> FileInfo::getSubDirectories(const std::string &basepath)
    {
        std::vector<std::string> ret;

        FileInfo searchPath(basepath, "*.*");
#ifdef WIN32
        struct _finddata_t c_file;
        intptr_t hFile;

        if ((hFile = _findfirst(searchPath.getFileName().c_str(), &c_file)) == -1L)
            return ret;

        do
        {
            if (c_file.attrib & _A_SUBDIR)
            {
                FileInfo file(basepath, c_file.name);
                ret.push_back(file.getFileName());
            }
        } while (_findnext(hFile, &c_file) == 0);
        _findclose(hFile);
#else
        DIR *dp = opendir(basepath.c_str());
        if (dp)
        {
            dirent *dirp;
            while ((dirp = readdir(dp)) != NULL)
            {
                FileInfo file(basepath, dirp->d_name);
                ret.push_back(file.getFileName());
            }
            closedir(dp);
        }
#endif

        ret.erase(std::remove_if(ret.begin(), ret.end(), [](const std::string& dirname) { return (dirname.empty() || (ccl::FileInfo(dirname).getBaseName() == ".") || (ccl::FileInfo(dirname).getBaseName() == "..")); }), ret.end());
        return ret;
    }

    bool directoryExists(const std::string &directory)
    {
#ifdef WIN32
        DWORD ftyp = GetFileAttributes(directory.c_str());
        if( ftyp == INVALID_FILE_ATTRIBUTES)
            return false;
        if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
            return true;
#else
    struct stat st;
    if(stat(directory.c_str(),&st) == 0)
        return S_ISDIR(st.st_mode);
#endif
        return false;
    }

    bool fileExists(const std::string filename)
    {
        if( access( filename.c_str(), 0 ) != -1 ) 
        {
            // file exists
            return true;
        } 
        else 
        {
            // file doesn't exist
            return false;
        }
    }

    bool FileInfo::fileExists(const std::string filename)
    {
        return ccl::fileExists(filename);
    }

    std::string joinPaths(const std::string &path1, const std::string &path2)
    {
        if(path1.empty())
            return path2;
        std::string newpath = path1;
        if((path1[path1.length()-1]!='\\')&&(path1[path1.length()-1]!='/'))
            newpath += "/";
        newpath += path2;
        return newpath;
    }
    
    std::string standardizeSlashes(const std::string &path)
    {
        std::string newpath = path;
        while(newpath.find("\\") != newpath.npos)
                newpath.replace(newpath.find("\\"), 1, "/");
        return newpath;
    }

    std::vector<std::string> splitPath(const std::string &path)
    {
        std::vector<std::string> ret;
        boost::split(ret, path, boost::is_any_of("\\/"));
        //Handle the special case where the path begins with a / (unix absolute paths)
        if(ret.size() > 0 && ret[0].empty())
            ret[0] = "/";
        return ret;
    }

    bool makeDirectory(const std::string &dir, bool makeIntermediate)
    {
        if(!makeIntermediate)
        {
#ifdef WIN32
            return 0==_mkdir(dir.c_str());
#else
            return 0==mkdir(dir.c_str(),S_IRWXU);
#endif
        }
        bool ret = true;
        std::vector<std::string> paths = splitPath(dir);
        std::string iterativePath;
        BOOST_FOREACH(std::string path,paths)
        {
            if(iterativePath.length())
                iterativePath = joinPaths(iterativePath,path);
            else
                iterativePath = path;
            if(!directoryExists(iterativePath))
            {
                ret &= makeDirectory(iterativePath,false);
            }
        }
        return ret;
    }

    bool deleteFile(const std::string &filename)
    {
#ifdef WIN32
      _unlink(filename.c_str());
#else
      unlink(filename.c_str());
#endif
      return true;
    }

  bool copyFile(const std::string &src, const std::string &dest)
  {
    try {
        std::ifstream source(src.c_str(), std::ios::binary);
        std::ofstream destf(dest.c_str(), std::ios::binary);

        destf << source.rdbuf();

        source.close();
        destf.close();
      return true;
    }
    catch(std::exception const &)
      {
    return false;
      }
  }

  bool copyFilesRecursive(const std::string &srcDir, const std::string &destDir)
  {
      std::vector<FileInfo> files = FileInfo::getAllFiles(srcDir, "*.*", true);
      for (auto&& fi : files)
      {
          std::string relativeFilename = fi.getDirName();
          //Strip out the source directory
          if (ccl::stringStartsWith(fi.getFileName(),srcDir))
          {
              relativeFilename = relativeFilename.substr(srcDir.length());
          }
          std::string destPath = ccl::joinPaths(destDir, relativeFilename);
          //Make sure the directory exists for each file
          makeDirectory(destPath, true);
          
          std::string destFilePath = ccl::joinPaths(destPath, fi.getBaseName());
          if (!copyFile(fi.getFileName(), destFilePath))
          {
              return false;
          }
      }

      return true;
  }

  uint64_t getFileSize(const std::string &filePath)
    {
        uint64_t flen = 0;
        FILE *file = fopen(filePath.c_str(),"rb");
        if(!file)
        return 0;
        fseek(file,0,SEEK_END);
        flen = ftell(file);
        fclose(file);
        return flen;
    }

  // Helper function to parse a table name after a GeoPackage filename
  // Returns true if a table name was detected after the .gpkg: string
  bool GetFilenameAndTable(const std::string &path, std::string &file, std::string &table)
  {
      const auto pos = path.find(".gpkg:");
      if (std::string::npos != pos)
      {
          //This is a geopackage file with the tablename included
          table = path.substr(pos + 6);
          file = path.substr(0, pos + 5);
          return true;
      }
      file = path;
      table = std::string();
      return false;
  }
}

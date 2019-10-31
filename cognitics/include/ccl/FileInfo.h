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
/*! \file ccl/FileInfo.h
\headerfile ccl/FileInfo.h
\brief Provides ccl::FileInfo.
\author Aaron Brinton <abrinton@cognitics.net>
\date 30 October 2009
*/
#pragma once

#include <string>
#include <vector>

namespace ccl
{
    /**
     * @class    FileInfo
     *
     * @brief    Utility class to split out parts of a file path
     */
    class FileInfo
    {
    private:
        std::string filename;

    public:
        ~FileInfo(void);
        FileInfo() {};

        FileInfo(const std::string &filename);

        /**
         * @fn    FileInfo::FileInfo(const std::string &basePath, const std::string &filepart);
         *
         * @brief    Constructor. Joins the path and file
         *
         * @param    basePath    Full pathname of the base file.
         * @param    filepart    The filepart.
         */

        FileInfo(const std::string &basePath, const std::string &filepart);

        /**
         * @fn    std::string FileInfo::getDirName(void);
         *
         * @brief    Gets the directory part of the filename only
         *
         * @return    The directory name from the specified path.
         */
        std::string getDirName(void);

        /**
         * @fn    std::string FileInfo::getBaseName(bool withoutSuffix = false);
         *
         * @brief    Gets the filename component of the path (without the directory)
         *
         * @param    withoutSuffix    (optional) return the filename portion without the suffix (including the .).
         *
         * @return    The base name.
         */
        std::string getBaseName(bool withoutSuffix = false) const;

        /**
         * @fn    std::string FileInfo::getSuffix(void);
         *
         * @brief    Gets the suffix without the dot.
         * @return    The suffix without the dot.
         */
        std::string getSuffix(void);

        /**
         * @fn    std::string FileInfo::getFileName();
         *
         * @brief    Gets file name including the path.
         *
         * @return    The file name including the path.
         */

        std::string getFileName() const;

        /**
         * @fn    static std::vector<FileInfo> FileInfo::getAllFiles(const std::string &basepath,
         *  const std::string &searchpattern);
         *
         * @brief    Gets all files matching the specified search pattern.
         * @param    basepath         The basepath.
         * @param    searchpattern    The searchpattern.
         * @param    subdir_search    If true, recurse all subdirectories of basepath in the search
         *
         * @return    all files found in the specified directory.
         */
        static std::vector<FileInfo> getAllFiles(const std::string &basepath, const std::string &searchpattern, bool subdir_search = false);

        /**
        * @fn    static std::vector<FileInfo> FileInfo::getSubDirectories(const std::string &basepath)
        *
        * @brief    Gets all subdirectories 
        * @param    basepath         The basepath.

        *
        * @return    all files found in the specified directory.
        */
        static std::vector<std::string> getSubDirectories(const std::string &basepath);

        /**
         * @fn    static bool FileInfo::fileExists(const std::string filename);
         *
         * @brief    Queries if a given file exists.
         *
         * @param    filename    Filename of the file.
         *
         * @return    true if it exists, false if it does not.
         */
        static bool fileExists(const std::string filename);

    };

    /**
     * @fn    std::string joinPaths(const std::string &path1, const std::string &path2);
     *
     * @brief    Join two parts of a file path, adding slashes between them if needed.
     *
     * @param    path1    The first path.
     * @param    path2    The second path.
     *
     * @return    the joined path
     */
    std::string joinPaths(const std::string &path1, const std::string &path2);

    /**
     * @fn    std::string standardizeSlashes(const std::string &path);
     *
     * @brief    Standardize slashes in the filename to be / instead of \\.
     *
     * @param    path    Full pathname of the file.
     *
     * @return    the path with / instead of \
     */
    std::string standardizeSlashes(const std::string &path);

    /**
     * @fn    bool makeDirectory(const std::string &dir, bool makeIntermediate=true);
     *
     * @brief    Make a directory, platform independent.
     *
     * @param    dir    Full directory name.
     * @param    makeIntermediate If true, create all necessary intermediate directories.
     *
     * @return    True on success, false on error.
     */
    bool makeDirectory(const std::string &dir, bool makeIntermediate=true);

    /**
     * @fn    std::vector<std::string> splitPath(const std::string &path)
     *
     * @brief    Split all the path components at all / or \\
     *
     * @param    path    Path to split
     *
     * @return    Vector of strings containing the path components without any slashes
     */
    std::vector<std::string> splitPath(const std::string &path);
    
    /**
      * @fn    static bool fileExists(const std::string filename);
     *
     * @brief    Queries if a given file exists.
     *
     * @param    filename    Filename of the file.
     *
     * @return    true if it exists, false if it does not.
     */
    bool fileExists(const std::string filename);
    /**
      * @fn    static bool directoryExists(const std::string filename);
     *
     * @brief    Queries if a given directory exists.
     *
     * @param    filename    Filename of the file.
     *
     * @return    true if it exists, false if it does not.
     */
    bool directoryExists(const std::string &directory);

    bool copyFile(const std::string &src, const std::string &dest);
    bool copyFilesRecursive(const std::string &srcDir, const std::string &destDir);

    uint64_t getFileSize(const std::string &filePath);
    bool deleteFile(const std::string &filename);
}

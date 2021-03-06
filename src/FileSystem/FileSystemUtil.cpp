/********************************************************************************************************\
*                                                                                                        *
*   This file is part of the Locus Game Engine                                                           *
*                                                                                                        *
*   Copyright (c) 2014 Shachar Avni. All rights reserved.                                                *
*                                                                                                        *
*   Use of this file is governed by a BSD-style license. See the accompanying LICENSE.txt for details    *
*                                                                                                        *
\********************************************************************************************************/

#include "Locus/FileSystem/FileSystemUtil.h"

#include "Locus/FileSystem/FileOnDisk.h"
#include "Locus/FileSystem/File.h"

#include "Locus/Common/Exception.h"
#include "Locus/Common/Parsing.h"

#if defined(LOCUS_WINDOWS)
   #define NOMINMAX
   #include <windows.h>
#elif defined(LOCUS_OSX)
   #include <stdlib.h>
#else
   #include <unistd.h>
#endif

#include <mutex>

namespace Locus
{

static std::string fullExePath;
static std::mutex getExePathMutex;

static std::string StripExeFromFullPath(const char* fullExePathIncludingExecutable)
{
   std::string exePath(fullExePathIncludingExecutable);

   #ifdef LOCUS_WINDOWS
      Locus::TrimUpToLastOccurenceOfChar(exePath, '\\');
   #else
      Locus::TrimUpToLastOccurenceOfChar(exePath, '/');
   #endif

   return exePath;
}

#if defined(LOCUS_WINDOWS)

static std::string GetExePath_Internal()
{
   char buffer[MAX_PATH + 1];
   if (GetModuleFileName(NULL, buffer, sizeof(buffer)) != 0)
   {
      return StripExeFromFullPath(buffer);
   }
   else
   {
      return "";
   }
}

bool GetAllFilesInDirectory(const std::string& directoryPath, std::vector<std::string>& filesInDirectory)
{
   filesInDirectory.clear();

   std::string directoryPathSearchString = directoryPath + "/*";

   WIN32_FIND_DATA findData;

   HANDLE findHandle = FindFirstFileA(directoryPathSearchString.c_str(), &findData);

   if (findHandle == INVALID_HANDLE_VALUE) 
   {
      return false;
   } 

   do
   {
      if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
      {
         filesInDirectory.push_back(findData.cFileName);
      }
   }
   while (FindNextFileA(findHandle, &findData) != 0);

   if (GetLastError() != ERROR_NO_MORE_FILES) 
   {
      return false;
   }

   return (FindClose(findHandle) == TRUE);
}

#elif defined(LOCUS_OSX)

static std::string GetExePath_Internal()
{
   std::vector<char> buffer(1024);
   uint32_t size = buffer.size();

   if (_NSGetExecutablePath(buffer.data(), &size) == 0)
   {
         return StripExeFromFullPath( realpath(buffer.data()) );
   }
   else
   {
      buffer.resize(size);
      if (_NSGetExecutablePath(buffer.data(), &size) == 0)
      {
            return StripExeFromFullPath( realpath(buffer.data()) );
      }
      else
      {
         return "";
      }
   }
}

//TODO
bool GetAllFilesInDirectory(const std::string& directoryPath, std::vector<std::string>& filesInDirectory)
{
   return false;
}

#else

static std::string GetExePath_Internal()
{
   char buffer[1024];
   ssize_t len = ::readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);

   if (len != -1)
   {
      buffer[len] = '\0';
      return StripExeFromFullPath(buffer);
   }
   else
   {
      return "";
   }
}

//TODO
bool GetAllFilesInDirectory(const std::string& directoryPath, std::vector<std::string>& filesInDirectory)
{
   return false;
}

#endif

std::string GetExePath()
{
   std::lock_guard<std::mutex> getExePathLockGuard(getExePathMutex);

   if (fullExePath.length() == 0)
   {
      fullExePath = GetExePath_Internal();

      if (fullExePath.length() == 0)
      {
         throw Exception("Failed to resolve path to the executable");
      }
   }

   return fullExePath;
}

void ReadWholeFile(const std::string& filePath, std::vector<char>& data)
{
   FileOnDisk fileOnDisk(filePath, DataStream::OpenMode::Read);

   fileOnDisk.ReadWholeFile(data);
}

void ReadWholeFile(const MountedFilePath& mountedFilePath, std::vector<char>& data)
{
   File file(mountedFilePath, DataStream::OpenMode::Read);

   file.ReadWholeFile(data);
}

}
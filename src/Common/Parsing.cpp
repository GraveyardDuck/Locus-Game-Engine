/********************************************************************************************************\
*                                                                                                        *
*   This file is part of the Locus Game Engine                                                           *
*                                                                                                        *
*   Copyright (c) 2014 Shachar Avni. All rights reserved.                                                *
*                                                                                                        *
*   Use of this file is governed by a BSD-style license. See the accompanying LICENSE.txt for details    *
*                                                                                                        *
\********************************************************************************************************/

#include "Locus/Common/Parsing.h"

#include <algorithm>

#include <cctype>

namespace Locus
{

void RemoveWhitespaces(std::string& str)
{
   str.erase(std::remove_if(str.begin(), str.end(), isspace), str.end());
}

void TrimString(std::string& str)
{
   std::istringstream trimmerStream(str);
   trimmerStream >> str;
}

void ToLower(std::string& str)
{
   std::transform(str.begin(), str.end(), str.begin(), ::tolower);
}

void ToUpper(std::string& str)
{
   std::transform(str.begin(), str.end(), str.begin(), ::toupper);
}

void TrimUpToLastOccurenceOfChar(std::string& str, char c)
{
   std::size_t indexOfLastC = str.rfind(c);

   if (indexOfLastC != std::string::npos)
   {
      str = str.substr(0, indexOfLastC + 1);
   }
}

}
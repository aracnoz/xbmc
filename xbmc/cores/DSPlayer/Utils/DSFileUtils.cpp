/*
 *      Copyright (C) 2005-2014 Team XBMC
 *      http://www.xbmc.org
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "utils/DSFileUtils.h"
#include "DSUtil/DSUtil.h"
#include "utils/charsetconverter.h"
#include "utils/StringUtils.h"

#ifdef HAS_DS_PLAYER

std::string CDSFile::SmbToUncPath(const std::string& strFileName)
{
  if (!StringUtils::StartsWithNoCase(strFileName, "smb://"))
    return strFileName;

  CStdString strWinFileName;
  std::size_t found = strFileName.find_first_of('@');

  if (found != std::string::npos)
  {
    strWinFileName = "\\\\" + strFileName.substr(found + 1);
  }
  else
  {
    strWinFileName = strFileName;
    strWinFileName.Replace("smb://", "\\\\");
  }

  strWinFileName.Replace('/', '\\');

  return strWinFileName;
}

bool CDSFile::Exists(const std::string& strFileName, long* errCode)
{
  CStdString strWinFile = SmbToUncPath(strFileName);
  CStdStringW strFileW;
  g_charsetConverter.utf8ToW(strWinFile, strFileW, false);

  DWORD dwAttr = GetFileAttributesW(strFileW.c_str());
  if(dwAttr != 0xffffffff)
    return true;

  if (errCode)
    *errCode = GetLastError();

  return false;
}

#endif

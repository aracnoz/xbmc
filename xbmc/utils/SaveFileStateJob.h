/*
 *  Copyright (C) 2010-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once
#ifdef HAS_DS_PLAYER
#include "cores/DSplayer/Filters/MadvrSettings.h"
#endif
class CBookmark;
class CFileItem;

class CSaveFileState
{
/* KRYPTON  CFileItem m_item;
  CFileItem m_item_discstack;
  CBookmark m_bookmark;
  bool      m_updatePlayCount;
  CVideoSettings m_videoSettings;
  CAudioSettings m_audioSettings;
  */
#ifdef HAS_DS_PLAYER
  CMadvrSettings m_madvrSettings;
#endif
/* KRYPTON
public:
                CSaveFileStateJob(const CFileItem& item,
                                  const CFileItem& item_discstack,
                                  const CBookmark& bookmark,
                                  bool updatePlayCount,
                                  const CVideoSettings &videoSettings,
                                  const CAudioSettings &audioSettings
                                  */
public:
#ifdef HAS_DS_PLAYER

                                  const CMadvrSettings &madvrSettings,
#endif
/* KRYPTON

                                  )
                  : m_item(item),
                    m_item_discstack(item_discstack),
                    m_bookmark(bookmark),
                    m_updatePlayCount(updatePlayCount),
                    m_videoSettings(videoSettings),
                    m_audioSettings(audioSettings)
                    */
#ifdef HAS_DS_PLAYER

                    m_madvrSettings(madvrSettings) {}
#endif

  static void DoWork(CFileItem& item,
                     CBookmark& bookmark,
                     bool updatePlayCount);
};

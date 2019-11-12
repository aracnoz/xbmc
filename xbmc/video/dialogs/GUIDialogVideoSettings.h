/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "cores/VideoPlayer/Interface/StreamInfo.h"
#include "settings/dialogs/GUIDialogSettingsManualBase.h"

#include <string>
#include <utility>
#include <vector>

#ifdef HAS_DS_PLAYER
#include "cores/DSPlayer/Dialogs/GUIDialogMadvrSettingsBase.h"
#include "DSPropertyPage.h"
#endif

struct IntegerSettingOption;

#ifdef HAS_DS_PLAYER
class CGUIDialogVideoSettings : public CGUIDialogMadvrSettingsBase
#else
class CGUIDialogVideoSettings : public CGUIDialogSettingsManualBase
#endif
{
public:
  CGUIDialogVideoSettings();
  ~CGUIDialogVideoSettings() override;

protected:
  // implementations of ISettingCallback
  void OnSettingChanged(std::shared_ptr<const CSetting> setting) override;
  void OnSettingAction(std::shared_ptr<const CSetting> setting) override;

  void AddVideoStreams(std::shared_ptr<CSettingGroup> group, const std::string & settingId);
  static void VideoStreamsOptionFiller(std::shared_ptr<const CSetting> setting, std::vector<IntegerSettingOption> &list, int &current, void *data);

  static void VideoOrientationFiller(std::shared_ptr<const CSetting> setting, std::vector<IntegerSettingOption> &list, int &current, void *data);

  static std::string FormatFlags(StreamFlags flags);

  // specialization of CGUIDialogSettingsBase
  bool AllowResettingSettings() const override { return false; }
  void Save() override;
  void SetupView() override;

  // specialization of CGUIDialogSettingsManualBase
  void InitializeSettings() override;

private:
  int m_videoStream;
#ifdef HAS_DS_PLAYER
  CDSPropertyPage* m_pDSPropertyPage;
  int m_scalingMethod;
  int m_dsStats;
#endif

  bool m_viewModeChanged = false;
};

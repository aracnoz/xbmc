/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "ButtonTranslator.h"

#include "AppTranslator.h"
#include "CustomControllerTranslator.h"
#include "FileItem.h"
#include "GamepadTranslator.h"
#include "IButtonMapper.h"
#include "IRTranslator.h"
#include "Key.h"
#include "KeyboardTranslator.h"
#include "Util.h"
#include "WindowTranslator.h"
#include "filesystem/Directory.h"
#include "guilib/WindowIDs.h"
#include "input/actions/ActionIDs.h"
#include "input/actions/ActionTranslator.h"
#include "input/mouse/MouseTranslator.h"
#include "utils/XBMCTinyXML.h"
// KRYPTON #include "XBIRRemote.h"

#ifdef HAS_DS_PLAYER
#include "settings/AdvancedSettings.h"
#endif
/* KRYPTON

using namespace XFILE;

typedef struct
{
  const char* name;
  int action;
} ActionMapping;

typedef struct
{
  int origin;
  int target;
} WindowMapping;

static const ActionMapping actions[] =
{
    { "left"                     , ACTION_MOVE_LEFT },
    { "right"                    , ACTION_MOVE_RIGHT },
    { "up"                       , ACTION_MOVE_UP },
    { "down"                     , ACTION_MOVE_DOWN },
    { "pageup"                   , ACTION_PAGE_UP },
    { "pagedown"                 , ACTION_PAGE_DOWN },
    { "select"                   , ACTION_SELECT_ITEM },
    { "highlight"                , ACTION_HIGHLIGHT_ITEM },
    { "parentdir"                , ACTION_NAV_BACK },                   // backward compatibility
    { "parentfolder"             , ACTION_PARENT_DIR },
    { "back"                     , ACTION_NAV_BACK },
    { "menu"                     , ACTION_MENU},
    { "previousmenu"             , ACTION_PREVIOUS_MENU },
    { "info"                     , ACTION_SHOW_INFO },
    { "pause"                    , ACTION_PAUSE },
    { "stop"                     , ACTION_STOP },
    { "skipnext"                 , ACTION_NEXT_ITEM },
    { "skipprevious"             , ACTION_PREV_ITEM },
    { "fullscreen"               , ACTION_SHOW_GUI },
    { "aspectratio"              , ACTION_ASPECT_RATIO },
    { "stepforward"              , ACTION_STEP_FORWARD },
    { "stepback"                 , ACTION_STEP_BACK },
    { "bigstepforward"           , ACTION_BIG_STEP_FORWARD },
    { "bigstepback"              , ACTION_BIG_STEP_BACK },
    { "chapterorbigstepforward"  , ACTION_CHAPTER_OR_BIG_STEP_FORWARD },
    { "chapterorbigstepback"     , ACTION_CHAPTER_OR_BIG_STEP_BACK },
    { "osd"                      , ACTION_SHOW_OSD },
    { "showsubtitles"            , ACTION_SHOW_SUBTITLES },
    { "nextsubtitle"             , ACTION_NEXT_SUBTITLE },
    { "cyclesubtitle"            , ACTION_CYCLE_SUBTITLE },
    { "playerdebug"              , ACTION_PLAYER_DEBUG },
    { "codecinfo"                , ACTION_PLAYER_PROCESS_INFO },
    { "playerprocessinfo"        , ACTION_PLAYER_PROCESS_INFO },
    */
#ifdef HAS_DS_PLAYER
    { "loaddsplayersettings1"       , ACTION_DSPLAYER_USERSETTINGS_1 },
    { "loaddsplayersettings2"       , ACTION_DSPLAYER_USERSETTINGS_2 },
    { "loaddsplayersettings3"       , ACTION_DSPLAYER_USERSETTINGS_3 },
    { "loaddsplayersettingssd"       , ACTION_DSPLAYER_USERSETTINGS_SD },
    { "loaddsplayersettings720"      , ACTION_DSPLAYER_USERSETTINGS_720 },
    { "loaddsplayersettings1080"     , ACTION_DSPLAYER_USERSETTINGS_1080 },
    { "loaddsplayersettings2160"     , ACTION_DSPLAYER_USERSETTINGS_2160 },
    { "loaddsplayersettingsatstart" , ACTION_DSPLAYER_USERSETTINGS_ATSTART },
#endif
/* KRYPTON
    { "nextpicture"              , ACTION_NEXT_PICTURE },
    { "previouspicture"          , ACTION_PREV_PICTURE },
    { "zoomout"                  , ACTION_ZOOM_OUT },
    { "zoomin"                   , ACTION_ZOOM_IN },
    { "playlist"                 , ACTION_SHOW_PLAYLIST },
    { "queue"                    , ACTION_QUEUE_ITEM },
    { "zoomnormal"               , ACTION_ZOOM_LEVEL_NORMAL },
    { "zoomlevel1"               , ACTION_ZOOM_LEVEL_1 },
    { "zoomlevel2"               , ACTION_ZOOM_LEVEL_2 },
    { "zoomlevel3"               , ACTION_ZOOM_LEVEL_3 },
    { "zoomlevel4"               , ACTION_ZOOM_LEVEL_4 },
    { "zoomlevel5"               , ACTION_ZOOM_LEVEL_5 },
    { "zoomlevel6"               , ACTION_ZOOM_LEVEL_6 },
    { "zoomlevel7"               , ACTION_ZOOM_LEVEL_7 },
    { "zoomlevel8"               , ACTION_ZOOM_LEVEL_8 },
    { "zoomlevel9"               , ACTION_ZOOM_LEVEL_9 },
    { "nextcalibration"          , ACTION_CALIBRATE_SWAP_ARROWS },
    { "resetcalibration"         , ACTION_CALIBRATE_RESET },
    { "analogmove"               , ACTION_ANALOG_MOVE },
    { "analogmovex"              , ACTION_ANALOG_MOVE_X },
    { "analogmovey"              , ACTION_ANALOG_MOVE_Y },
    { "rotate"                   , ACTION_ROTATE_PICTURE_CW },
    { "rotateccw"                , ACTION_ROTATE_PICTURE_CCW },
    { "close"                    , ACTION_NAV_BACK },                    // backwards compatibility
    { "subtitledelayminus"       , ACTION_SUBTITLE_DELAY_MIN },
    { "subtitledelay"            , ACTION_SUBTITLE_DELAY },
    { "subtitledelayplus"        , ACTION_SUBTITLE_DELAY_PLUS },
    { "audiodelayminus"          , ACTION_AUDIO_DELAY_MIN },
    { "audiodelay"               , ACTION_AUDIO_DELAY },
    { "audiodelayplus"           , ACTION_AUDIO_DELAY_PLUS },
    { "subtitleshiftup"          , ACTION_SUBTITLE_VSHIFT_UP },
    { "subtitleshiftdown"        , ACTION_SUBTITLE_VSHIFT_DOWN },
    { "subtitlealign"            , ACTION_SUBTITLE_ALIGN },
    { "audionextlanguage"        , ACTION_AUDIO_NEXT_LANGUAGE },
    { "verticalshiftup"          , ACTION_VSHIFT_UP },
    { "verticalshiftdown"        , ACTION_VSHIFT_DOWN },
    { "nextresolution"           , ACTION_CHANGE_RESOLUTION },
    { "audiotoggledigital"       , ACTION_TOGGLE_DIGITAL_ANALOG },
    { "number0"                  , REMOTE_0 },
    { "number1"                  , REMOTE_1 },
    { "number2"                  , REMOTE_2 },
    { "number3"                  , REMOTE_3 },
    { "number4"                  , REMOTE_4 },
    { "number5"                  , REMOTE_5 },
    { "number6"                  , REMOTE_6 },
    { "number7"                  , REMOTE_7 },
    { "number8"                  , REMOTE_8 },
    { "number9"                  , REMOTE_9 },
    { "smallstepback"            , ACTION_SMALL_STEP_BACK },
    { "fastforward"              , ACTION_PLAYER_FORWARD },
    { "rewind"                   , ACTION_PLAYER_REWIND },
    { "play"                     , ACTION_PLAYER_PLAY },
    { "playpause"                , ACTION_PLAYER_PLAYPAUSE },
    { "switchplayer"             , ACTION_SWITCH_PLAYER },
    { "delete"                   , ACTION_DELETE_ITEM },
    { "copy"                     , ACTION_COPY_ITEM },
    { "move"                     , ACTION_MOVE_ITEM },
    { "screenshot"               , ACTION_TAKE_SCREENSHOT },
    { "rename"                   , ACTION_RENAME_ITEM },
    { "togglewatched"            , ACTION_TOGGLE_WATCHED },
    { "scanitem"                 , ACTION_SCAN_ITEM },
    { "reloadkeymaps"            , ACTION_RELOAD_KEYMAPS },
    { "volumeup"                 , ACTION_VOLUME_UP },
    { "volumedown"               , ACTION_VOLUME_DOWN },
    { "mute"                     , ACTION_MUTE },
    { "backspace"                , ACTION_BACKSPACE },
    { "scrollup"                 , ACTION_SCROLL_UP },
    { "scrolldown"               , ACTION_SCROLL_DOWN },
    { "analogfastforward"        , ACTION_ANALOG_FORWARD },
    { "analogrewind"             , ACTION_ANALOG_REWIND },
    { "moveitemup"               , ACTION_MOVE_ITEM_UP },
    { "moveitemdown"             , ACTION_MOVE_ITEM_DOWN },
    { "contextmenu"              , ACTION_CONTEXT_MENU },
    { "shift"                    , ACTION_SHIFT },
    { "symbols"                  , ACTION_SYMBOLS },
    { "cursorleft"               , ACTION_CURSOR_LEFT },
    { "cursorright"              , ACTION_CURSOR_RIGHT },
    { "showtime"                 , ACTION_SHOW_OSD_TIME },
    { "analogseekforward"        , ACTION_ANALOG_SEEK_FORWARD },
    { "analogseekback"           , ACTION_ANALOG_SEEK_BACK },
    { "showpreset"               , ACTION_VIS_PRESET_SHOW },
    { "nextpreset"               , ACTION_VIS_PRESET_NEXT },
    { "previouspreset"           , ACTION_VIS_PRESET_PREV },
    { "lockpreset"               , ACTION_VIS_PRESET_LOCK },
    { "randompreset"             , ACTION_VIS_PRESET_RANDOM },
    { "increasevisrating"        , ACTION_VIS_RATE_PRESET_PLUS },
    { "decreasevisrating"        , ACTION_VIS_RATE_PRESET_MINUS },
    { "showvideomenu"            , ACTION_SHOW_VIDEOMENU },
    { "enter"                    , ACTION_ENTER },
    { "increaserating"           , ACTION_INCREASE_RATING },
    { "decreaserating"           , ACTION_DECREASE_RATING },
    { "setrating"                , ACTION_SET_RATING },
    { "togglefullscreen"         , ACTION_TOGGLE_FULLSCREEN },
    { "nextscene"                , ACTION_NEXT_SCENE },
    { "previousscene"            , ACTION_PREV_SCENE },
    { "nextletter"               , ACTION_NEXT_LETTER },
    { "prevletter"               , ACTION_PREV_LETTER },
    { "jumpsms2"                 , ACTION_JUMP_SMS2 },
    { "jumpsms3"                 , ACTION_JUMP_SMS3 },
    { "jumpsms4"                 , ACTION_JUMP_SMS4 },
    { "jumpsms5"                 , ACTION_JUMP_SMS5 },
    { "jumpsms6"                 , ACTION_JUMP_SMS6 },
    { "jumpsms7"                 , ACTION_JUMP_SMS7 },
    { "jumpsms8"                 , ACTION_JUMP_SMS8 },
    { "jumpsms9"                 , ACTION_JUMP_SMS9 },
    { "filter"                   , ACTION_FILTER },
    { "filterclear"              , ACTION_FILTER_CLEAR },
    { "filtersms2"               , ACTION_FILTER_SMS2 },
    { "filtersms3"               , ACTION_FILTER_SMS3 },
    { "filtersms4"               , ACTION_FILTER_SMS4 },
    { "filtersms5"               , ACTION_FILTER_SMS5 },
    { "filtersms6"               , ACTION_FILTER_SMS6 },
    { "filtersms7"               , ACTION_FILTER_SMS7 },
    { "filtersms8"               , ACTION_FILTER_SMS8 },
    { "filtersms9"               , ACTION_FILTER_SMS9 },
    { "firstpage"                , ACTION_FIRST_PAGE },
    { "lastpage"                 , ACTION_LAST_PAGE },
    { "guiprofile"               , ACTION_GUIPROFILE_BEGIN },
    { "red"                      , ACTION_TELETEXT_RED },
    { "green"                    , ACTION_TELETEXT_GREEN },
    { "yellow"                   , ACTION_TELETEXT_YELLOW },
    { "blue"                     , ACTION_TELETEXT_BLUE },
    { "increasepar"              , ACTION_INCREASE_PAR },
    { "decreasepar"              , ACTION_DECREASE_PAR },
    { "volampup"                 , ACTION_VOLAMP_UP },
    { "volampdown"               , ACTION_VOLAMP_DOWN },
    { "volumeamplification"      , ACTION_VOLAMP },
    { "createbookmark"           , ACTION_CREATE_BOOKMARK },
    { "createepisodebookmark"    , ACTION_CREATE_EPISODE_BOOKMARK },
    { "settingsreset"            , ACTION_SETTINGS_RESET },
    { "settingslevelchange"      , ACTION_SETTINGS_LEVEL_CHANGE },

    // 3D movie playback/GUI
    { "stereomode"               , ACTION_STEREOMODE_SELECT },          // cycle 3D modes, for now an alias for next
    { "nextstereomode"           , ACTION_STEREOMODE_NEXT },
    { "previousstereomode"       , ACTION_STEREOMODE_PREVIOUS },
    { "togglestereomode"         , ACTION_STEREOMODE_TOGGLE },
    { "stereomodetomono"         , ACTION_STEREOMODE_TOMONO },

    // PVR actions
    { "channelup"                , ACTION_CHANNEL_UP },
    { "channeldown"              , ACTION_CHANNEL_DOWN },
    { "previouschannelgroup"     , ACTION_PREVIOUS_CHANNELGROUP },
    { "nextchannelgroup"         , ACTION_NEXT_CHANNELGROUP },
    { "playpvr"                  , ACTION_PVR_PLAY },
    { "playpvrtv"                , ACTION_PVR_PLAY_TV },
    { "playpvrradio"             , ACTION_PVR_PLAY_RADIO },
    { "record"                   , ACTION_RECORD },
    { "togglecommskip"           , ACTION_TOGGLE_COMMSKIP },
    { "showtimerrule"            , ACTION_PVR_SHOW_TIMER_RULE },

    // Mouse actions
    { "leftclick"                , ACTION_MOUSE_LEFT_CLICK },
    { "rightclick"               , ACTION_MOUSE_RIGHT_CLICK },
    { "middleclick"              , ACTION_MOUSE_MIDDLE_CLICK },
    { "doubleclick"              , ACTION_MOUSE_DOUBLE_CLICK },
    { "longclick"                , ACTION_MOUSE_LONG_CLICK },
    { "wheelup"                  , ACTION_MOUSE_WHEEL_UP },
    { "wheeldown"                , ACTION_MOUSE_WHEEL_DOWN },
    { "mousedrag"                , ACTION_MOUSE_DRAG },
    { "mousemove"                , ACTION_MOUSE_MOVE },

    // Touch
    { "tap"                      , ACTION_TOUCH_TAP },
    { "longpress"                , ACTION_TOUCH_LONGPRESS },
    { "pangesture"               , ACTION_GESTURE_PAN },
    { "zoomgesture"              , ACTION_GESTURE_ZOOM },
    { "rotategesture"            , ACTION_GESTURE_ROTATE },
    { "swipeleft"                , ACTION_GESTURE_SWIPE_LEFT },
    { "swiperight"               , ACTION_GESTURE_SWIPE_RIGHT },
    { "swipeup"                  , ACTION_GESTURE_SWIPE_UP },
    { "swipedown"                , ACTION_GESTURE_SWIPE_DOWN },

    // Do nothing / error action
    { "error"                    , ACTION_ERROR },
    { "noop"                     , ACTION_NOOP }
};

bool CButtonTranslator::IsAnalog(int actionID)
{
  switch (actionID)
  {
  case ACTION_ANALOG_SEEK_FORWARD:
  case ACTION_ANALOG_SEEK_BACK:
  case ACTION_SCROLL_UP:
  case ACTION_SCROLL_DOWN:
  case ACTION_ANALOG_FORWARD:
  case ACTION_ANALOG_REWIND:
  case ACTION_ANALOG_MOVE:
  case ACTION_ANALOG_MOVE_X:
  case ACTION_ANALOG_MOVE_Y:
  case ACTION_CURSOR_LEFT:
  case ACTION_CURSOR_RIGHT:
  case ACTION_VOLUME_UP:
  case ACTION_VOLUME_DOWN:
  case ACTION_ZOOM_IN:
  case ACTION_ZOOM_OUT:
    return true;
  default:
    return false;
  }
}

static const ActionMapping windows[] =
{
    { "home"                     , WINDOW_HOME },
    { "programs"                 , WINDOW_PROGRAMS },
    { "pictures"                 , WINDOW_PICTURES },
    { "filemanager"              , WINDOW_FILES },
    { "settings"                 , WINDOW_SETTINGS_MENU },
    { "music"                    , WINDOW_MUSIC_NAV },
    { "videos"                   , WINDOW_VIDEO_NAV },
    { "tvchannels"               , WINDOW_TV_CHANNELS },
    { "tvrecordings"             , WINDOW_TV_RECORDINGS },
    { "tvguide"                  , WINDOW_TV_GUIDE },
    { "tvtimers"                 , WINDOW_TV_TIMERS },
    { "tvsearch"                 , WINDOW_TV_SEARCH },
    { "radiochannels"            , WINDOW_RADIO_CHANNELS },
    { "radiorecordings"          , WINDOW_RADIO_RECORDINGS },
    { "radioguide"               , WINDOW_RADIO_GUIDE },
    { "radiotimers"              , WINDOW_RADIO_TIMERS },
    { "radiosearch"              , WINDOW_RADIO_SEARCH },
    { "gamecontrollers"          , WINDOW_DIALOG_GAME_CONTROLLERS },
    { "pvrguideinfo"             , WINDOW_DIALOG_PVR_GUIDE_INFO },
    { "pvrrecordinginfo"         , WINDOW_DIALOG_PVR_RECORDING_INFO },
    { "pvrradiordsinfo"          , WINDOW_DIALOG_PVR_RADIO_RDS_INFO },
    { "pvrtimersetting"          , WINDOW_DIALOG_PVR_TIMER_SETTING },
    { "pvrgroupmanager"          , WINDOW_DIALOG_PVR_GROUP_MANAGER },
    { "pvrchannelmanager"        , WINDOW_DIALOG_PVR_CHANNEL_MANAGER },
    { "pvrguidesearch"           , WINDOW_DIALOG_PVR_GUIDE_SEARCH },
    { "pvrchannelscan"           , WINDOW_DIALOG_PVR_CHANNEL_SCAN },
    { "pvrupdateprogress"        , WINDOW_DIALOG_PVR_UPDATE_PROGRESS },
    { "pvrosdchannels"           , WINDOW_DIALOG_PVR_OSD_CHANNELS },
    { "pvrosdguide"              , WINDOW_DIALOG_PVR_OSD_GUIDE },
    { "pvrosdteletext"           , WINDOW_DIALOG_OSD_TELETEXT },
    { "systeminfo"               , WINDOW_SYSTEM_INFORMATION },
    { "testpattern"              , WINDOW_TEST_PATTERN },
    { "screencalibration"        , WINDOW_SCREEN_CALIBRATION },
    { "systemsettings"           , WINDOW_SETTINGS_SYSTEM },
    { "servicesettings"          , WINDOW_SETTINGS_SERVICE },
    { "pvrsettings"              , WINDOW_SETTINGS_MYPVR },
    { "playersettings"           , WINDOW_SETTINGS_PLAYER },
    { "mediasettings"            , WINDOW_SETTINGS_MEDIA },
    { "interfacesettings"        , WINDOW_SETTINGS_INTERFACE },
    { "appearancesettings"       , WINDOW_SETTINGS_INTERFACE },	// backward compatibility to v16
    { "videoplaylist"            , WINDOW_VIDEO_PLAYLIST },
    { "loginscreen"              , WINDOW_LOGIN_SCREEN },
    { "profiles"                 , WINDOW_SETTINGS_PROFILES },
    { "skinsettings"             , WINDOW_SKIN_SETTINGS },
    { "addonbrowser"             , WINDOW_ADDON_BROWSER },
    { "yesnodialog"              , WINDOW_DIALOG_YES_NO },
    { "progressdialog"           , WINDOW_DIALOG_PROGRESS },
    { "virtualkeyboard"          , WINDOW_DIALOG_KEYBOARD },
    { "volumebar"                , WINDOW_DIALOG_VOLUME_BAR },
    { "submenu"                  , WINDOW_DIALOG_SUB_MENU },
    { "favourites"               , WINDOW_DIALOG_FAVOURITES },
    { "contextmenu"              , WINDOW_DIALOG_CONTEXT_MENU },
    { "notification"             , WINDOW_DIALOG_KAI_TOAST },
    { "numericinput"             , WINDOW_DIALOG_NUMERIC },
    { "gamepadinput"             , WINDOW_DIALOG_GAMEPAD },
    { "shutdownmenu"             , WINDOW_DIALOG_BUTTON_MENU },
    { "playercontrols"           , WINDOW_DIALOG_PLAYER_CONTROLS },
    { "playerprocessinfo"        , WINDOW_DIALOG_PLAYER_PROCESS_INFO },
    { "seekbar"                  , WINDOW_DIALOG_SEEK_BAR },
    { "musicosd"                 , WINDOW_DIALOG_MUSIC_OSD },
    { "addonsettings"            , WINDOW_DIALOG_ADDON_SETTINGS },
    { "visualisationpresetlist"  , WINDOW_DIALOG_VIS_PRESET_LIST },
    { "osdcmssettings"           , WINDOW_DIALOG_CMS_OSD_SETTINGS },
    { "osdvideosettings"         , WINDOW_DIALOG_VIDEO_OSD_SETTINGS },
    { "osdaudiosettings"         , WINDOW_DIALOG_AUDIO_OSD_SETTINGS },

    { "audiodspmanager"          , WINDOW_DIALOG_AUDIO_DSP_MANAGER },
    { "osdaudiodspsettings"      , WINDOW_DIALOG_AUDIO_DSP_OSD_SETTINGS },
    { "videobookmarks"           , WINDOW_DIALOG_VIDEO_BOOKMARKS },
    { "filebrowser"              , WINDOW_DIALOG_FILE_BROWSER },
    { "networksetup"             , WINDOW_DIALOG_NETWORK_SETUP },
    { "mediasource"              , WINDOW_DIALOG_MEDIA_SOURCE },
    { "profilesettings"          , WINDOW_DIALOG_PROFILE_SETTINGS },
    { "locksettings"             , WINDOW_DIALOG_LOCK_SETTINGS },
    { "contentsettings"          , WINDOW_DIALOG_CONTENT_SETTINGS },
    { "songinformation"          , WINDOW_DIALOG_SONG_INFO },
    { "smartplaylisteditor"      , WINDOW_DIALOG_SMART_PLAYLIST_EDITOR },
    { "smartplaylistrule"        , WINDOW_DIALOG_SMART_PLAYLIST_RULE },
    { "busydialog"               , WINDOW_DIALOG_BUSY },
    { "pictureinfo"              , WINDOW_DIALOG_PICTURE_INFO },
    { "accesspoints"             , WINDOW_DIALOG_ACCESS_POINTS },
    { "fullscreeninfo"           , WINDOW_DIALOG_FULLSCREEN_INFO },
    { "sliderdialog"             , WINDOW_DIALOG_SLIDER },
    { "addoninformation"         , WINDOW_DIALOG_ADDON_INFO },
    { "subtitlesearch"           , WINDOW_DIALOG_SUBTITLES },
    { "musicplaylist"            , WINDOW_MUSIC_PLAYLIST },
    { "musicplaylisteditor"      , WINDOW_MUSIC_PLAYLIST_EDITOR },
    { "teletext"                 , WINDOW_DIALOG_OSD_TELETEXT },
    { "selectdialog"             , WINDOW_DIALOG_SELECT },
    { "musicinformation"         , WINDOW_DIALOG_MUSIC_INFO },
    { "okdialog"                 , WINDOW_DIALOG_OK },
    { "movieinformation"         , WINDOW_DIALOG_VIDEO_INFO },
    { "textviewer"               , WINDOW_DIALOG_TEXT_VIEWER },
    { "fullscreenvideo"          , WINDOW_FULLSCREEN_VIDEO },
    { "fullscreenlivetv"         , WINDOW_FULLSCREEN_LIVETV },         // virtual window/keymap section for PVR specific bindings in fullscreen playback (which internally uses WINDOW_FULLSCREEN_VIDEO)
    { "fullscreenradio"          , WINDOW_FULLSCREEN_RADIO },          // virtual window for fullscreen radio, uses WINDOW_VISUALISATION as fallback
    { "visualisation"            , WINDOW_VISUALISATION },
    { "slideshow"                , WINDOW_SLIDESHOW },*/
#ifdef HAS_DS_PLAYER
    { "dsrulesettings"           , WINDOW_DIALOG_DSRULES },
    { "dsfiltersettings"         , WINDOW_DIALOG_DSFILTERS },
    { "dsplayercoresettings"     , WINDOW_DIALOG_DSPLAYERCORE },
    { "dsplayerlavvideo"         , WINDOW_DIALOG_LAVVIDEO },
    { "dsplayerlavaudio"         , WINDOW_DIALOG_LAVAUDIO },
    { "dsplayerlavsplitter"      , WINDOW_DIALOG_LAVSPLITTER },
    { "dsplayersanear"           , WINDOW_DIALOG_SANEAR },
    { "dsplayerprocessinfo"      , WINDOW_DIALOG_DSPLAYER_PROCESS_INFO },
#endif
/* KRYPTON
    { "weather"                  , WINDOW_WEATHER },
    { "screensaver"              , WINDOW_SCREENSAVER },
    { "videoosd"                 , WINDOW_DIALOG_VIDEO_OSD },
    { "videomenu"                , WINDOW_VIDEO_MENU },
    { "videotimeseek"            , WINDOW_VIDEO_TIME_SEEK },
    { "startwindow"              , WINDOW_START },
    { "startup"                  , WINDOW_STARTUP_ANIM },
    { "peripheralsettings"       , WINDOW_DIALOG_PERIPHERAL_SETTINGS },
    { "extendedprogressdialog"   , WINDOW_DIALOG_EXT_PROGRESS },
    { "mediafilter"              , WINDOW_DIALOG_MEDIA_FILTER },
    { "addon"                    , WINDOW_ADDON_START },
    { "eventlog"                 , WINDOW_EVENT_LOG},
    { "tvtimerrules"             , WINDOW_TV_TIMER_RULES},
    { "radiotimerrules"          , WINDOW_RADIO_TIMER_RULES}
};

static const ActionMapping mousekeys[] =
{
    { "click"                    , KEY_MOUSE_CLICK },
    { "leftclick"                , KEY_MOUSE_CLICK },
    { "rightclick"               , KEY_MOUSE_RIGHTCLICK },
    { "middleclick"              , KEY_MOUSE_MIDDLECLICK },
    { "doubleclick"              , KEY_MOUSE_DOUBLE_CLICK },
    { "longclick"                , KEY_MOUSE_LONG_CLICK },
    { "wheelup"                  , KEY_MOUSE_WHEEL_UP },
    { "wheeldown"                , KEY_MOUSE_WHEEL_DOWN },
    { "mousemove"                , KEY_MOUSE_MOVE },
    { "mousedrag"                , KEY_MOUSE_DRAG },
    { "mousedragstart"           , KEY_MOUSE_DRAG_START },
    { "mousedragend"             , KEY_MOUSE_DRAG_END },
    { "mouserdrag"               , KEY_MOUSE_RDRAG },
    { "mouserdragstart"          , KEY_MOUSE_RDRAG_START },
    { "mouserdragend"            , KEY_MOUSE_RDRAG_END }
};

static const ActionMapping touchcommands[] =
{
    { "tap"                      , ACTION_TOUCH_TAP },
    { "longpress"                , ACTION_TOUCH_LONGPRESS },
    { "pan"                      , ACTION_GESTURE_PAN },
    { "zoom"                     , ACTION_GESTURE_ZOOM },
    { "rotate"                   , ACTION_GESTURE_ROTATE },
    { "swipeleft"                , ACTION_GESTURE_SWIPE_LEFT },
    { "swiperight"               , ACTION_GESTURE_SWIPE_RIGHT },
    { "swipeup"                  , ACTION_GESTURE_SWIPE_UP },
    { "swipedown"                , ACTION_GESTURE_SWIPE_DOWN }
};

static const WindowMapping fallbackWindows[] =
{
    { WINDOW_FULLSCREEN_LIVETV   , WINDOW_FULLSCREEN_VIDEO },
    { WINDOW_FULLSCREEN_RADIO    , WINDOW_VISUALISATION }
};

#ifdef TARGET_WINDOWS
static const ActionMapping appcommands[] =
{
    { "browser_back"             , APPCOMMAND_BROWSER_BACKWARD },
    { "browser_forward"          , APPCOMMAND_BROWSER_FORWARD },
    { "browser_refresh"          , APPCOMMAND_BROWSER_REFRESH },
    { "browser_stop"             , APPCOMMAND_BROWSER_STOP },
    { "browser_search"           , APPCOMMAND_BROWSER_SEARCH },
    { "browser_favorites"        , APPCOMMAND_BROWSER_FAVORITES },
    { "browser_home"             , APPCOMMAND_BROWSER_HOME },
    { "volume_mute"              , APPCOMMAND_VOLUME_MUTE },
    { "volume_down"              , APPCOMMAND_VOLUME_DOWN },
    { "volume_up"                , APPCOMMAND_VOLUME_UP },
    { "next_track"               , APPCOMMAND_MEDIA_NEXTTRACK },
    { "prev_track"               , APPCOMMAND_MEDIA_PREVIOUSTRACK },
    { "stop"                     , APPCOMMAND_MEDIA_STOP },
    { "play_pause"               , APPCOMMAND_MEDIA_PLAY_PAUSE },
    { "launch_mail"              , APPCOMMAND_LAUNCH_MAIL },
    { "launch_media_select"      , APPCOMMAND_LAUNCH_MEDIA_SELECT },
    { "launch_app1"              , APPCOMMAND_LAUNCH_APP1 },
    { "launch_app2"              , APPCOMMAND_LAUNCH_APP2 },
    { "play"                     , APPCOMMAND_MEDIA_PLAY },
    { "pause"                    , APPCOMMAND_MEDIA_PAUSE },
    { "fastforward"              , APPCOMMAND_MEDIA_FAST_FORWARD },
    { "rewind"                   , APPCOMMAND_MEDIA_REWIND },
    { "channelup"                , APPCOMMAND_MEDIA_CHANNEL_UP },
    { "channeldown"              , APPCOMMAND_MEDIA_CHANNEL_DOWN }
};
#endif

CButtonTranslator& CButtonTranslator::GetInstance()
{
  static CButtonTranslator sl_instance;
  return sl_instance;
}*/


#include "utils/log.h"

#include <algorithm>
#include <utility>

using namespace KODI;

// Add the supplied device name to the list of connected devices
bool CButtonTranslator::AddDevice(const std::string& strDevice)
{
  // Only add the device if it isn't already in the list
  if (m_deviceList.find(strDevice) != m_deviceList.end())
    return false;

  // Add the device
  m_deviceList.insert(strDevice);

  // New device added so reload the key mappings
  Load();

  return true;
}

bool CButtonTranslator::RemoveDevice(const std::string& strDevice)
{
  // Find the device
  auto it = m_deviceList.find(strDevice);
  if (it == m_deviceList.end())
    return false;

  // Remove the device
  m_deviceList.erase(it);

  // Device removed so reload the key mappings
  Load();

  return true;
}

bool CButtonTranslator::Load()
{
  Clear();

  // Directories to search for keymaps. They're applied in this order,
  // so keymaps in profile/keymaps/ override e.g. system/keymaps
  static std::vector<std::string> DIRS_TO_CHECK = {
    "special://xbmc/system/keymaps/",
    "special://masterprofile/keymaps/",
    "special://profile/keymaps/"
  };

  bool success = false;

  for (const auto& dir : DIRS_TO_CHECK)
  {
    if (XFILE::CDirectory::Exists(dir))
    {
      CFileItemList files;
      XFILE::CDirectory::GetDirectory(dir, files, ".xml", XFILE::DIR_FLAG_DEFAULTS);
      // Sort the list for filesystem based priorities, e.g. 01-keymap.xml, 02-keymap-overrides.xml
      files.Sort(SortByFile, SortOrderAscending);
      for(int fileIndex = 0; fileIndex<files.Size(); ++fileIndex)
      {
        if (!files[fileIndex]->m_bIsFolder)
          success |= LoadKeymap(files[fileIndex]->GetPath());
      }

      // Load mappings for any HID devices we have connected
      for (const auto& device : m_deviceList)
      {
        std::string devicedir = dir;
        devicedir.append(device);
        devicedir.append("/");
        if (XFILE::CDirectory::Exists(devicedir))
        {
          CFileItemList files;
          XFILE::CDirectory::GetDirectory(devicedir, files, ".xml", XFILE::DIR_FLAG_DEFAULTS);
          // Sort the list for filesystem based priorities, e.g. 01-keymap.xml, 02-keymap-overrides.xml
          files.Sort(SortByFile, SortOrderAscending);
          for(int fileIndex = 0; fileIndex<files.Size(); ++fileIndex)
          {
            if (!files[fileIndex]->m_bIsFolder)
              success |= LoadKeymap(files[fileIndex]->GetPath());
          }
        }
      }
    }
  }

  if (!success)
  {
    CLog::Log(LOGERROR, "Error loading keymaps from: %s or %s or %s",
      DIRS_TO_CHECK[0].c_str(), DIRS_TO_CHECK[1].c_str(), DIRS_TO_CHECK[2].c_str());
    return false;
  }

/* KRYPTON
#if defined(HAS_LIRC) || defined(HAS_IRSERVERSUITE)
#ifdef TARGET_POSIX
#define REMOTEMAP "Lircmap.xml"
#else
#define REMOTEMAP "IRSSmap.xml"
#endif
  std::string lircmapPath = URIUtils::AddFileToFolder("special://xbmc/system/", REMOTEMAP);
  lircRemotesMap.clear();
  if(CFile::Exists(lircmapPath))
    success |= LoadLircMap(lircmapPath);
  else
    CLog::Log(LOGDEBUG, "CButtonTranslator::Load - no system %s found, skipping", REMOTEMAP);

  lircmapPath = CProfilesManager::GetInstance().GetUserDataItem(REMOTEMAP);
  if(CFile::Exists(lircmapPath))
    success |= LoadLircMap(lircmapPath);
  else
    CLog::Log(LOGDEBUG, "CButtonTranslator::Load - no userdata %s found, skipping", REMOTEMAP);

  if (!success)
    CLog::Log(LOGERROR, "CButtonTranslator::Load - unable to load remote map %s", REMOTEMAP);
  // don't return false - it is to only indicate a fatal error (which this is not)
#endif
*/ 
#ifdef HAS_DS_PLAYER
  if (!m_translatorMap[WINDOW_DIALOG_PLAYER_PROCESS_INFO].empty())
    m_translatorMap[WINDOW_DIALOG_DSPLAYER_PROCESS_INFO] = m_translatorMap[WINDOW_DIALOG_PLAYER_PROCESS_INFO];
#endif

  // Done!
  return true;
}

bool CButtonTranslator::LoadKeymap(const std::string &keymapPath)
{
#ifdef HAS_DS_PLAYER
  if (g_advancedSettings.m_bIgnoreSystemAppcommand
    && keymapPath.find("system") != keymapPath.npos
    && (keymapPath.find("appcommand") != keymapPath.npos
      || keymapPath.find("gamepad") != keymapPath.npos
      || keymapPath.find("joystick") != keymapPath.npos)
    )
    return false;
#endif

  CXBMCTinyXML xmlDoc;

  CLog::Log(LOGINFO, "Loading %s", keymapPath.c_str());
  if (!xmlDoc.LoadFile(keymapPath))
  {
    CLog::Log(LOGERROR, "Error loading keymap: %s, Line %d\n%s", keymapPath.c_str(), xmlDoc.ErrorRow(), xmlDoc.ErrorDesc());
    return false;
  }

  TiXmlElement* pRoot = xmlDoc.RootElement();
  if (pRoot == nullptr)
  {
    CLog::Log(LOGERROR, "Error getting keymap root: %s", keymapPath.c_str());
    return false;
  }

  std::string strValue = pRoot->Value();
  if (strValue != "keymap")
  {
    CLog::Log(LOGERROR, "%s Doesn't contain <keymap>", keymapPath.c_str());
    return false;
  }

  // run through our window groups
  TiXmlNode* pWindow = pRoot->FirstChild();
  while (pWindow != nullptr)
  {
    if (pWindow->Type() == TiXmlNode::TINYXML_ELEMENT)
    {
      int windowID = WINDOW_INVALID;
      const char *szWindow = pWindow->Value();
      if (szWindow != nullptr)
      {
        if (strcmpi(szWindow, "global") == 0)
          windowID = -1;
        else
          windowID = CWindowTranslator::TranslateWindow(szWindow);
      }
      MapWindowActions(pWindow, windowID);
    }
    pWindow = pWindow->NextSibling();
  }

  return true;
}

CAction CButtonTranslator::GetAction(int window, const CKey &key, bool fallback)
{
  std::string strAction;

  // handle virtual windows
  window = CWindowTranslator::GetVirtualWindow(window);

  // try to get the action from the current window
  unsigned int actionID = GetActionCode(window, key, strAction);

  if (fallback)
  {
    // if it's invalid, try to get it from fallback windows or the global map (window == -1)
    while (actionID == ACTION_NONE && window > -1)
    {
      window = CWindowTranslator::GetFallbackWindow(window);
      actionID = GetActionCode(window, key, strAction);
    }
  }

  return CAction(actionID, strAction, key);
}

bool CButtonTranslator::HasLongpressMapping(int window, const CKey &key)
{
  // handle virtual windows
  window = CWindowTranslator::GetVirtualWindow(window);
  return HasLongpressMapping_Internal(window, key);
}

bool CButtonTranslator::HasLongpressMapping_Internal(int window, const CKey &key)
{
  std::map<int, buttonMap>::const_iterator it = m_translatorMap.find(window);
  if (it != m_translatorMap.end())
  {
    uint32_t code = key.GetButtonCode();
    code |= CKey::MODIFIER_LONG;
    buttonMap::const_iterator it2 = (*it).second.find(code);

    if (it2 != (*it).second.end())
      return it2->second.id != ACTION_NOOP;

#ifdef TARGET_POSIX
    // Some buttoncodes changed in Hardy
    if ((code & KEY_VKEY) == KEY_VKEY && (code & 0x0F00))
    {
      code &= ~0x0F00;
      it2 = (*it).second.find(code);
      if (it2 != (*it).second.end())
        return true;
    }
#endif
  }

  // no key mapping found for the current window do the fallback handling
  if (window > -1)
  {
    // first check if we have a fallback for the window
    int fallbackWindow = CWindowTranslator::GetFallbackWindow(window);
    if (fallbackWindow > -1 && HasLongpressMapping_Internal(fallbackWindow, key))
      return true;

    // fallback to default section
    return HasLongpressMapping_Internal(-1, key);
  }

  return false;
}

unsigned int CButtonTranslator::GetActionCode(int window, const CKey &key, std::string &strAction) const
{
  uint32_t code = key.GetButtonCode();

  std::map<int, buttonMap>::const_iterator it = m_translatorMap.find(window);
  if (it == m_translatorMap.end())
    return ACTION_NONE;

  buttonMap::const_iterator it2 = (*it).second.find(code);
  unsigned int action = ACTION_NONE;
  if (it2 == (*it).second.end() && code & CKey::MODIFIER_LONG) // If long action not found, try short one
  {
    code &= ~CKey::MODIFIER_LONG;
    it2 = (*it).second.find(code);
  }

  if (it2 != (*it).second.end())
  {
    action = (*it2).second.id;
    strAction = (*it2).second.strID;
  }

#ifdef TARGET_POSIX
  // Some buttoncodes changed in Hardy
  if (action == ACTION_NONE && (code & KEY_VKEY) == KEY_VKEY && (code & 0x0F00))
  {
    CLog::Log(LOGDEBUG, "%s: Trying Hardy keycode for %#04x", __FUNCTION__, code);
    code &= ~0x0F00;
    it2 = (*it).second.find(code);
    if (it2 != (*it).second.end())
    {
      action = (*it2).second.id;
      strAction = (*it2).second.strID;
    }
  }
#endif

  return action;
}

void CButtonTranslator::MapAction(uint32_t buttonCode, const std::string &szAction, buttonMap &map)
{
  unsigned int action = ACTION_NONE;
  if (!CActionTranslator::TranslateString(szAction, action) || buttonCode == 0)
    return;   // no valid action, or an invalid buttoncode

  // have a valid action, and a valid button - map it.
  // check to see if we've already got this (button,action) pair defined
  buttonMap::iterator it = map.find(buttonCode);
  if (it == map.end() || (*it).second.id != action || (*it).second.strID != szAction)
  {
    // NOTE: This multimap is only being used as a normal map at this point (no support
    //       for multiple actions per key)
    if (it != map.end())
      map.erase(it);
    CButtonAction button;
    button.id = action;
    button.strID = szAction;
    map.insert(std::pair<uint32_t, CButtonAction>(buttonCode, button));
  }
}

void CButtonTranslator::MapWindowActions(const TiXmlNode *pWindow, int windowID)
{
  if (pWindow == nullptr || windowID == WINDOW_INVALID)
    return;

  const TiXmlNode *pDevice;

  static const std::vector<std::string> types = {"gamepad", "remote", "universalremote", "keyboard", "mouse", "appcommand"};

  for (const auto& type : types)
  {
    for (pDevice = pWindow->FirstChild(type);
         pDevice != nullptr;
         pDevice = pDevice->NextSiblingElement(type))
    {
      buttonMap map;
      std::map<int, buttonMap>::iterator it = m_translatorMap.find(windowID);
      if (it != m_translatorMap.end())
      {
        map = std::move(it->second);
        m_translatorMap.erase(it);
      }

      const TiXmlElement *pButton = pDevice->FirstChildElement();

      while (pButton != nullptr)
      {
        uint32_t buttonCode = 0;

        if (type == "gamepad")
            buttonCode = CGamepadTranslator::TranslateString(pButton->Value());
        else if (type == "remote")
            buttonCode = CIRTranslator::TranslateString(pButton->Value());
        else if (type == "universalremote")
            buttonCode = CIRTranslator::TranslateUniversalRemoteString(pButton->Value());
        else if (type == "keyboard")
            buttonCode = CKeyboardTranslator::TranslateButton(pButton);
        else if (type == "mouse")
            buttonCode = CMouseTranslator::TranslateCommand(pButton);
        else if (type == "appcommand")
            buttonCode = CAppTranslator::TranslateAppCommand(pButton->Value());

        if (buttonCode != 0)
        {
          if (pButton->FirstChild() && pButton->FirstChild()->Value()[0])
            MapAction(buttonCode, pButton->FirstChild()->Value(), map);
          else
          {
            buttonMap::iterator it = map.find(buttonCode);
            while (it != map.end())
            {
              map.erase(it);
              it = map.find(buttonCode);
            }
          }
        }
        pButton = pButton->NextSiblingElement();
      }

      // add our map to our table
      if (!map.empty())
        m_translatorMap.insert(std::make_pair(windowID, std::move(map)));
    }
  }

  for (auto it : m_buttonMappers)
  {
    const std::string &device = it.first;
    IButtonMapper *mapper = it.second;

    // Map device actions
    pDevice = pWindow->FirstChild(device);
    while (pDevice != nullptr)
    {
      mapper->MapActions(windowID, pDevice);
      pDevice = pDevice->NextSibling(device);
    }
  }
}

void CButtonTranslator::Clear()
{
  m_translatorMap.clear();

  for (auto it : m_buttonMappers)
    it.second->Clear();
}

void CButtonTranslator::RegisterMapper(const std::string &device, IButtonMapper *mapper)
{
  m_buttonMappers[device] = mapper;
}

void CButtonTranslator::UnregisterMapper(IButtonMapper *mapper)
{
  for (auto it = m_buttonMappers.begin(); it != m_buttonMappers.end(); ++it)
  {
    if (it->second == mapper)
    {
      m_buttonMappers.erase(it);
      break;
    }
  }
}

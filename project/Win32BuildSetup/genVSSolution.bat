@ECHO OFF
SETLOCAL ENABLEDELAYEDEXPANSION
REM setup all paths
SET cur_dir=%CD%
SET base_dir=%cur_dir%\..\..
SET awk_exe=%msys_dir%\usr\bin\awk.exe
SET sed_exe=%msys_dir%\usr\bin\sed.exe

REM read the version values from version.txt
FOR /f %%i IN ('%awk_exe% "/APP_NAME/ {print $2}" %base_dir%\version.txt') DO SET APP_NAME=%%i
FOR /f %%i IN ('%awk_exe% "/COMPANY_NAME/ {print $2}" %base_dir%\version.txt') DO SET COMPANY_NAME=%%i
FOR /f %%i IN ('%awk_exe% "/WEBSITE/ {print $2}" %base_dir%\version.txt') DO SET WEBSITE=%%i
FOR /f %%i IN ('%awk_exe% "/VERSION_MAJOR/ {print $2}" %base_dir%\version.txt') DO SET MAJOR=%%i
FOR /f %%i IN ('%awk_exe% "/VERSION_MINOR/ {print $2}" %base_dir%\version.txt') DO SET MINOR=%%i
FOR /f %%i IN ('%awk_exe% "/VERSION_TAG/ {print $2}" %base_dir%\version.txt') DO SET TAG=%%i
FOR /f %%i IN ('%awk_exe% "/ADDON_API/ {print $2}" %base_dir%\version.txt') DO SET VERSION_NUMBER=%%i.0

SET APP_VERSION=%MAJOR%.%MINOR%
IF NOT [%TAG%] == [] (
  SET APP_VERSION=%APP_VERSION%-%TAG%
)

rem ----Usage----
rem genVSSolution [clean|noclean]
rem clean to force a full rebuild
rem noclean to force a creation without clean
CLS
COLOR 1B
TITLE %APP_NAME% for Windows Build Script
rem ----PURPOSE----
rem - Create Visual Studio Solution
rem -------------------------------------------------------------
rem  CONFIG START
SET createmode=ask
SET exitcode=0
SET useshell=rxvt
SET BRANCH=na
FOR %%b in (%1) DO (
  IF %%b==clean SET createdmode=clean
  IF %%b==noclean SET createmode=noclean
)

set WORKSPACE=%CD%\..\..

  :: sets the BRANCH env var
  call getbranch.bat

  rem  CONFIG END
  rem -------------------------------------------------------------

  ECHO Wait while preparing the build.
  ECHO ------------------------------------------------------------
  ECHO Generating Visual Studio Solution %APP_NAME% branch %BRANCH%...

  IF %createmode%==clean (
    RMDIR /S /Q %WORKSPACE%\kodi-build
  )
  MKDIR %WORKSPACE%\kodi-build
  PUSHD %WORKSPACE%\kodi-build

  cmake.exe -G "Visual Studio 14 2015 Win64" %base_dir%\project\cmake
  IF %errorlevel%==1 (
    set DIETEXT="%APP_NAME%.EXE failed to build!"
    goto DIE
  )
  goto END

:DIE
  ECHO ------------------------------------------------------------
  ECHO !-!-!-!-!-!-!-!-!-!-!-!-!-!-!-!-!-!-!-!-!-!-!-!-!-!-!-!-!-!-
  ECHO    ERROR ERROR ERROR ERROR ERROR ERROR ERROR ERROR ERROR
  ECHO !-!-!-!-!-!-!-!-!-!-!-!-!-!-!-!-!-!-!-!-!-!-!-!-!-!-!-!-!-!-
  set DIETEXT=ERROR: %DIETEXT%
  echo %DIETEXT%
  SET exitcode=1
  ECHO ------------------------------------------------------------
  GOTO END

:END
  ECHO Press any key to exit...
  pause > NUL

  EXIT /B %exitcode%

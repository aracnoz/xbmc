# - Builds DSPlayer dependencies as external project
# Once done this will define
#
# DSEXTERNALS_FOUND - system has DSEXTERNALS
# DSEXTERNALS_LIBRARIES

include(ExternalProject)

if (CMAKE_SIZEOF_VOID_P EQUAL 8)
  set (DS_PLATFORM x64)
else ()
  set (DS_PLATFORM Win32)
endif ()

ExternalProject_Add(dsexternals
            SOURCE_DIR "${CORE_SOURCE_DIR}/xbmc/cores/dsplayer/libs/"
            PREFIX "${CORE_SOURCE_DIR}/xbmc/cores/dsplayer/libs/"
            CONFIGURE_COMMAND ""
            BUILD_COMMAND msbuild "${CORE_SOURCE_DIR}/xbmc/cores/dsplayer/libs/dsplayer_externals.sln"
                                  /p:Configuration=${CORE_BUILD_CONFIG} /p:Platform=${DS_PLATFORM}
            INSTALL_COMMAND ""
            BUILD_BYPRODUCTS "${DSUTIL_LIBRARY_DEBUG} ${DSUTIL_LIBRARY_RELEASE} ${SUBPIC_LIBRARY_DEBUG} ${SUBPIC_LIBRARY_RELEASE}")
set_target_properties(dsexternals PROPERTIES FOLDER "External Projects")

set(DSUTIL_LIBRARY_DEBUG "${CORE_SOURCE_DIR}/kodi-build/build/dsutil/Debug/dsutild.lib" )
set(DSUTIL_LIBRARY_RELEASE "${CORE_SOURCE_DIR}/kodi-build/build/dsutil/Release/dsutil.lib")
set(SUBPIC_LIBRARY_DEBUG "${CORE_SOURCE_DIR}/kodi-build/build/subpic/Debug/subpicd.lib" )
set(SUBPIC_LIBRARY_RELEASE "${CORE_SOURCE_DIR}/kodi-build/build/subpic/Release/subpic.lib" )

set(DSEXTERNALS_FOUND 1)

include(SelectLibraryConfigurations)
select_library_configurations(DSUTIL)
select_library_configurations(SUBPIC)

set(DSEXTERNALS_LIBRARIES ${DSUTIL_LIBRARY} ${SUBPIC_LIBRARY})

mark_as_advanced(DSEXTERNALS_FOUND DSUTIL_LIBRARY SUBPIC_LIBRARY)

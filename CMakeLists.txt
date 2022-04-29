cmake_minimum_required(VERSION 3.19)
project(ivan CXX C)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
message("CMAKE_SOURCE_DIR is ${CMAKE_SOURCE_DIR}")

set(VERSION_MAJOR 0)
set(VERSION_MINOR 50)
set(VERSION_PATCH 0)
set(PROJECT_VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
add_definitions(-DIVAN_VERSION="${PROJECT_VERSION}" -DUSE_SDL)

option(BUILD_MAC_APP "Build standalone application for MacOS" OFF)
option(USE_HOME_FOR_STATE_DIR "Statedir will be /.ivan/ in current user's homedir" OFF)

option(WIZARD "Enable Wizard Mode" OFF)

# -O0 -g3
# -s ASYNCIFY -s ASYNCIFY_IGNORE_INDIRECT

if(EMSCRIPTEN)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ffast-math -m32 -O3 -g3 -sUSE_SDL=2 -sALLOW_MEMORY_GROWTH=1 -sDISABLE_EXCEPTION_CATCHING=0 -sDISABLE_EXCEPTION_THROWING=0 -sEXPORTED_RUNTIME_METHODS=[ccall] -lidbfs.js")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffast-math -m32 -O3 -g3 -sUSE_SDL=2 -sDISABLE_EXCEPTION_CATCHING=0 -sDISABLE_EXCEPTION_THROWING=0")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -m32 -sWASM=1 -sASYNCIFY -sALLOW_MEMORY_GROWTH=1 -sDISABLE_EXCEPTION_CATCHING=0 -sDISABLE_EXCEPTION_THROWING=0 -sEXPORTED_RUNTIME_METHODS=[ccall] -lidbfs.js --emrun")
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
endif()

if(UNIX)
  add_definitions(-DUNIX)
  include(GNUInstallDirs)

  if(BUILD_MAC_APP)
    install(DIRECTORY Graphics Script DESTINATION "ivan")
    install(FILES COPYING INSTALL LICENSING NEWS README DESTINATION "docs")
    add_definitions(-DMAC_APP)
  else()
    install(DIRECTORY Graphics Script DESTINATION "${CMAKE_INSTALL_DATADIR}/ivan")
    if(EMSCRIPTEN)
      install(FILES Main/JS/loadFile.js DESTINATION "${CMAKE_INSTALL_DATADIR}/ivan")
    endif()
    add_definitions(-DDATADIR="${CMAKE_INSTALL_FULL_DATADIR}")

    message("\n*NOTICE*\n"
            "    Highscore files and Bone files are now stored at \"\$HOME/.ivan\".\n"
            "    Your HOME directory is \"$ENV{HOME}\".\n")
    if(IS_DIRECTORY "${CMAKE_INSTALL_FULL_LOCALSTATEDIR}/ivan")
      message(WARNING
              "If you have game data in \"${CMAKE_INSTALL_FULL_LOCALSTATEDIR}/ivan\","
              " please move them to \"$ENV{HOME}/.ivan\".")
    endif()
  endif()

elseif(WIN32)
  install(DIRECTORY Graphics Script DESTINATION "ivan")
  install(FILES COPYING INSTALL LICENSING NEWS README DESTINATION "ivan")
  add_definitions(-DWIN32)

  if(MINGW)
    set(CMAKE_EXE_LINKER_FLAGS -mwindows)
  endif(MINGW)

  # This is bad. To remove below defines, FeLib should switch to std::chrono and serialize time since common epoch.
  # 32 bit TIME_T has year 2038 problem. Moreover, serialized value is not portable for time_t in general.
  # Switching to std::chrono should not be hard, but it's not a topic for this pull request.
  add_definitions(-D_USE_32BIT_TIME_T)
endif(UNIX)

#add_definitions(-D_USE_32BIT_TIME_T)

# In visual studio, add an option to hide the console window (useful where debugging is not intended - i.e. releases)
if(MSVC AND MSVC_HIDE_CONSOLE_WINDOW)
  # Hide the console window in visual studio projects
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
endif()

if(WIZARD)
  add_definitions(-DWIZARD)
endif()

add_subdirectory(FeLib)
add_subdirectory(Main)

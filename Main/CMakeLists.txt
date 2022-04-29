#include(FindPkgConfig)
#if(PKG_CONFIG_FOUND)
#  pkg_check_modules(PCRE libpcre)
#  set(PCRE_LIBRARIES ${PCRE_LDFLAGS})
#endif()
#if(NOT PCRE_FOUND)
#  if(MSVC)
#    find_path(PCRE_INCLUDE_DIR NAMES pcre.h)
#    find_library(PCRE_LIBRARY pcre)
#  endif()
#  find_package(PCRE REQUIRED)
#  add_definitions(${PCRE_DEFINITIONS})
#endif()

#find_package(SDL2)
find_package(SDL2 REQUIRED)
#find_file(SDL2_INCLUDE_DIR NAME SDL.h HINTS SDL2)
#find_library(SDL2_LIBRARY NAME SDL2)

file(GLOB IVAN_SOURCES Include/*.h Source/*.cpp)
# The following files are #included from other .cpp files and so shouldn't be
# compiled directly. Setting the HEADER_FILE_ONLY property to TRUE prevents it.
set_source_files_properties(
    Source/action.cpp   Source/actions.cpp   Source/area.cpp     Source/bodypart.cpp
    Source/char.cpp     Source/cont.cpp      Source/database.cpp Source/entity.cpp
    Source/fluid.cpp    Source/gear.cpp      Source/god.cpp      Source/gods.cpp
    Source/human.cpp    Source/item.cpp      Source/level.cpp    Source/lsquare.cpp
    Source/lterra.cpp   Source/lterras.cpp   Source/materia.cpp  Source/materias.cpp
    Source/miscitem.cpp Source/nonhuman.cpp  Source/pool.cpp     Source/proto.cpp
    Source/rain.cpp     Source/room.cpp      Source/rooms.cpp    Source/slot.cpp
    Source/smoke.cpp    Source/square.cpp    Source/stack.cpp    Source/team.cpp
    Source/terra.cpp    Source/trap.cpp      Source/traps.cpp    Source/worldmap.cpp
    Source/wsquare.cpp  Source/wterra.cpp    Source/wterras.cpp
    PROPERTIES HEADER_FILE_ONLY TRUE)

add_executable(ivan ${IVAN_SOURCES} Resource/Ivan.rc)
target_include_directories(ivan PUBLIC Include ../Felib/Include ${PCRE_INCLUDE_DIRS})
target_link_libraries(ivan FeLib ${PCRE_LIBRARIES})

if(${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    configure_file(${CMAKE_SOURCE_DIR}/Main/Resource/favicon.ico ${CMAKE_CURRENT_BINARY_DIR}/favicon.ico COPYONLY)
    file(COPY ${CMAKE_SOURCE_DIR}/Graphics DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    file(COPY ${CMAKE_SOURCE_DIR}/Script DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    set_target_properties(ivan PROPERTIES LINK_FLAGS "-o ivan.js -s DISABLE_EXCEPTION_CATCHING=0 -s DISABLE_EXCEPTION_THROWING=0 -s DEMANGLE_SUPPORT=1 -s SAFE_HEAP=0 --bind -s WASM=1 -s ASSERTIONS=1 --preload-file Graphics --preload-file Script --preload-file favicon.ico")
endif()

if(MSVC AND _VCPKG_INSTALLED_DIR)
  # Gum solution. Manually copy pcre.dll to where ivan.exe will end up.
  install(FILES "${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/bin/pcre.dll" DESTINATION "ivan")
endif()

if(UNIX)
    install(TARGETS ivan DESTINATION "${CMAKE_INSTALL_BINDIR}")
else()
    install(TARGETS ivan DESTINATION "ivan")
endif()

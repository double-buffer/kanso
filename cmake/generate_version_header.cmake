execute_process(
  COMMAND git describe --tags --abbrev=0
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_TAG
  OUTPUT_STRIP_TRAILING_WHITESPACE
  ERROR_QUIET
)

if(NOT GIT_TAG)
  set(GIT_TAG "0.0.0-DEV")
endif()

string(REGEX MATCH "^([0-9]+)\\.([0-9]+)\\.([0-9]+)-([A-Za-z0-9]+)$" _ "${GIT_TAG}")

set(MAJOR ${CMAKE_MATCH_1})
set(MINOR ${CMAKE_MATCH_2})
math(EXPR BUILD "${CMAKE_MATCH_3} + 1")
set(LABEL ${CMAKE_MATCH_4})

set(VERSION_FULL "${MAJOR}.${MINOR}.${BUILD}-${LABEL}")

file(WRITE "${OUT_FILE}" "
// Auto-generated version.h from Git tag ${GIT_TAG}

#pragma once

#define KANSO_VERSION_MAJOR ${MAJOR}
#define KANSO_VERSION_MINOR ${MINOR}
#define KANSO_VERSION_BUILD ${BUILD}
#define KANSO_VERSION_LABEL \"${LABEL}\"
#define KANSO_VERSION_FULL \"${VERSION_FULL}\"
")


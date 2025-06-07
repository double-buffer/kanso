function(get_git_version OUT_PREFIX)
  execute_process(
    COMMAND git describe --tags --abbrev=0
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE _GIT_TAG
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
  )

  if(NOT _GIT_TAG)
    set(_GIT_TAG "0.0.0-UNKNOWN")
  endif()

  string(REGEX MATCH "^([0-9]+)\\.([0-9]+)\\.([0-9]+)-([A-Za-z0-9]+)$" _ "${_GIT_TAG}")

  set(${OUT_PREFIX}_MAJOR "${CMAKE_MATCH_1}" PARENT_SCOPE)
  set(${OUT_PREFIX}_MINOR "${CMAKE_MATCH_2}" PARENT_SCOPE)
  set(${OUT_PREFIX}_BUILD "${CMAKE_MATCH_3}" PARENT_SCOPE)
  set(${OUT_PREFIX}_LABEL "${CMAKE_MATCH_4}" PARENT_SCOPE)

  set(${OUT_PREFIX}_FULL
      "${CMAKE_MATCH_1}.${CMAKE_MATCH_2}.${CMAKE_MATCH_3}-${CMAKE_MATCH_4}"
      PARENT_SCOPE
  )
endfunction()


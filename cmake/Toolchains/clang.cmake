include_guard(GLOBAL)

if(NOT CMAKE_C_COMPILER OR NOT CMAKE_ASM_COMPILER)

    execute_process(
        COMMAND brew --prefix llvm
        OUTPUT_VARIABLE _BREW_LLVM_PREFIX
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )

    find_program(_CLANG
        NAMES clang-20 clang-19 clang
        HINTS "${_BREW_LLVM_PREFIX}/bin"
              "C:\\Program Files\\LLVM\\bin"
              "/usr/bin"
        NO_CMAKE_SYSTEM_PATH
    )

    if(NOT _CLANG)
        message(FATAL_ERROR "Could not find Clang.")
    endif()

    set(CMAKE_C_COMPILER   "${_CLANG}" CACHE FILEPATH "C compiler" FORCE)
    set(CMAKE_ASM_COMPILER "${_CLANG}" CACHE FILEPATH "ASM compiler" FORCE)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif()

find_package(Git QUIET)

if (GIT_FOUND AND EXISTS ${PROJECT_SOURCE_DIR}/.git)
  
  option(GIT_SUBMODULE "Check submodules during build" ON)

  if (GIT_SUBMODULE)

    message(STATUS "Submodule update")
    execute_process(
      COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
      RESULT_VARIABLE GIT_SUBMODULE_RESULT
    )

    if (NOT GIT_SUBMOD_RESULT EQUAL "0")
      message(
        FATAL_ERROR
        "Updating GitSM failed with ${GIT_SUBMODULE_RESULT}"
      )
    endif()

  endif()

endif()

if (NOT EXISTS "${PROJECT_SOURCE_DIR}/ext/googletest/CMakeLists.txt"})
  message(FATAL_ERROR "Failed to clone GitSM")
endif()

option(BUILD_GMOCK "Disable unecessary GTest build" OFF)



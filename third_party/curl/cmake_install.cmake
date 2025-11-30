# Install script for directory: /Users/Peyilo/Development/Code/Clion/booksource/third_party/curl

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set path to fallback-tool for dependency-resolution.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/Peyilo/Development/Code/Clion/booksource/third_party/curl/docs/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/Peyilo/Development/Code/Clion/booksource/third_party/curl/scripts/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/Peyilo/Development/Code/Clion/booksource/third_party/curl/lib/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/Peyilo/Development/Code/Clion/booksource/third_party/curl/src/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/Peyilo/Development/Code/Clion/booksource/third_party/curl/docs/examples/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/Peyilo/Development/Code/Clion/booksource/third_party/curl/tests/cmake_install.cmake")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE FILE PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES "/Users/Peyilo/Development/Code/Clion/booksource/third_party/curl/curl-config")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" TYPE FILE FILES "/Users/Peyilo/Development/Code/Clion/booksource/third_party/curl/libcurl.pc")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE DIRECTORY FILES "/Users/Peyilo/Development/Code/Clion/booksource/third_party/curl/include/curl" FILES_MATCHING REGEX "/[^/]*\\.h$")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/CURL/CURLTargets.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/CURL/CURLTargets.cmake"
         "/Users/Peyilo/Development/Code/Clion/booksource/third_party/curl/CMakeFiles/Export/8e83d16133499b505bf3986f4f209a65/CURLTargets.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/CURL/CURLTargets-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/CURL/CURLTargets.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/CURL" TYPE FILE FILES "/Users/Peyilo/Development/Code/Clion/booksource/third_party/curl/CMakeFiles/Export/8e83d16133499b505bf3986f4f209a65/CURLTargets.cmake")
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^()$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/CURL" TYPE FILE FILES "/Users/Peyilo/Development/Code/Clion/booksource/third_party/curl/CMakeFiles/Export/8e83d16133499b505bf3986f4f209a65/CURLTargets-noconfig.cmake")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/CURL" TYPE FILE FILES
    "/Users/Peyilo/Development/Code/Clion/booksource/third_party/curl/generated/CURLConfigVersion.cmake"
    "/Users/Peyilo/Development/Code/Clion/booksource/third_party/curl/generated/CURLConfig.cmake"
    "/Users/Peyilo/Development/Code/Clion/booksource/third_party/curl/CMake/FindBrotli.cmake"
    "/Users/Peyilo/Development/Code/Clion/booksource/third_party/curl/CMake/FindCares.cmake"
    "/Users/Peyilo/Development/Code/Clion/booksource/third_party/curl/CMake/FindGSS.cmake"
    "/Users/Peyilo/Development/Code/Clion/booksource/third_party/curl/CMake/FindGnuTLS.cmake"
    "/Users/Peyilo/Development/Code/Clion/booksource/third_party/curl/CMake/FindLDAP.cmake"
    "/Users/Peyilo/Development/Code/Clion/booksource/third_party/curl/CMake/FindLibbacktrace.cmake"
    "/Users/Peyilo/Development/Code/Clion/booksource/third_party/curl/CMake/FindLibgsasl.cmake"
    "/Users/Peyilo/Development/Code/Clion/booksource/third_party/curl/CMake/FindLibidn2.cmake"
    "/Users/Peyilo/Development/Code/Clion/booksource/third_party/curl/CMake/FindLibpsl.cmake"
    "/Users/Peyilo/Development/Code/Clion/booksource/third_party/curl/CMake/FindLibrtmp.cmake"
    "/Users/Peyilo/Development/Code/Clion/booksource/third_party/curl/CMake/FindLibssh.cmake"
    "/Users/Peyilo/Development/Code/Clion/booksource/third_party/curl/CMake/FindLibssh2.cmake"
    "/Users/Peyilo/Development/Code/Clion/booksource/third_party/curl/CMake/FindLibuv.cmake"
    "/Users/Peyilo/Development/Code/Clion/booksource/third_party/curl/CMake/FindMbedTLS.cmake"
    "/Users/Peyilo/Development/Code/Clion/booksource/third_party/curl/CMake/FindNGHTTP2.cmake"
    "/Users/Peyilo/Development/Code/Clion/booksource/third_party/curl/CMake/FindNGHTTP3.cmake"
    "/Users/Peyilo/Development/Code/Clion/booksource/third_party/curl/CMake/FindNGTCP2.cmake"
    "/Users/Peyilo/Development/Code/Clion/booksource/third_party/curl/CMake/FindNettle.cmake"
    "/Users/Peyilo/Development/Code/Clion/booksource/third_party/curl/CMake/FindQuiche.cmake"
    "/Users/Peyilo/Development/Code/Clion/booksource/third_party/curl/CMake/FindRustls.cmake"
    "/Users/Peyilo/Development/Code/Clion/booksource/third_party/curl/CMake/FindWolfSSL.cmake"
    "/Users/Peyilo/Development/Code/Clion/booksource/third_party/curl/CMake/FindZstd.cmake"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE FILE PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES "/Users/Peyilo/Development/Code/Clion/booksource/third_party/curl/scripts/wcurl")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "/Users/Peyilo/Development/Code/Clion/booksource/third_party/curl/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()

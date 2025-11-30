#***************************************************************************
#                                  _   _ ____  _
#  Project                     ___| | | |  _ \| |
#                             / __| | | | |_) | |
#                            | (__| |_| |  _ <| |___
#                             \___|\___/|_| \_\_____|
#
# Copyright (C) Daniel Stenberg, <daniel@haxx.se>, et al.
#
# This software is licensed as described in the file COPYING, which
# you should have received as part of this distribution. The terms
# are also available at https://curl.se/docs/copyright.html.
#
# You may opt to use, copy, modify, merge, publish, distribute and/or sell
# copies of the Software, and permit persons to whom the Software is
# furnished to do so, under the terms of the COPYING file.
#
# This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
# KIND, either express or implied.
#
# SPDX-License-Identifier: curl
#
###########################################################################

####### Expanded from @PACKAGE_INIT@ by configure_package_config_file() #######
####### Any changes to this file will be overwritten by the next CMake run ####
####### The input file was curl-config.cmake.in                            ########

get_filename_component(PACKAGE_PREFIX_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../" ABSOLUTE)

macro(set_and_check _var _file)
  set(${_var} "${_file}")
  if(NOT EXISTS "${_file}")
    message(FATAL_ERROR "File or directory ${_file} referenced by variable ${_var} does not exist !")
  endif()
endmacro()

macro(check_required_components _NAME)
  foreach(comp ${${_NAME}_FIND_COMPONENTS})
    if(NOT ${_NAME}_${comp}_FOUND)
      if(${_NAME}_FIND_REQUIRED_${comp})
        set(${_NAME}_FOUND FALSE)
      endif()
    endif()
  endforeach()
endmacro()

####################################################################################

option(CURL_USE_PKGCONFIG "Enable pkg-config to detect CURL dependencies. Default: ON" "ON")

include(CMakeFindDependencyMacro)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR} ${CMAKE_MODULE_PATH})

set(_libs "")
if("ON")
  if("3")
    find_dependency(OpenSSL "3")
  else()
    find_dependency(OpenSSL)
  endif()
endif()
if("ON")
  find_dependency(ZLIB "1")
endif()
if("ON")
  find_dependency(Brotli)
  list(APPEND _libs CURL::brotli)
endif()
if("")
  find_dependency(Cares)
  list(APPEND _libs CURL::cares)
endif()
if("")
  find_dependency(GSS)
  list(APPEND _libs CURL::gss)
endif()
if("")
  find_dependency(Libbacktrace)
  list(APPEND _libs CURL::libbacktrace)
endif()
if("")
  find_dependency(Libgsasl)
  list(APPEND _libs CURL::libgsasl)
endif()
if(NOT "" AND NOT "OFF")
  find_dependency(LDAP)
  list(APPEND _libs CURL::ldap)
endif()
if("1")
  find_dependency(Libidn2)
  list(APPEND _libs CURL::libidn2)
endif()
if("ON")
  find_dependency(Libpsl)
  list(APPEND _libs CURL::libpsl)
endif()
if("OFF")
  find_dependency(Librtmp)
  list(APPEND _libs CURL::librtmp)
endif()
if("")
  find_dependency(Libssh)
  list(APPEND _libs CURL::libssh)
endif()
if("ON")
  find_dependency(Libssh2)
  list(APPEND _libs CURL::libssh2)
endif()
if("")
  find_dependency(Libuv)
  list(APPEND _libs CURL::libuv)
endif()
if("")
  find_dependency(MbedTLS)
  list(APPEND _libs CURL::mbedtls)
endif()
if("ON")
  find_dependency(NGHTTP2)
  list(APPEND _libs CURL::nghttp2)
endif()
if("")
  find_dependency(NGHTTP3)
  list(APPEND _libs CURL::nghttp3)
endif()
if("OFF")
  find_dependency(NGTCP2)
  list(APPEND _libs CURL::ngtcp2)
endif()
if("")
  find_dependency(GnuTLS)
  list(APPEND _libs CURL::gnutls)
  find_dependency(Nettle)
  list(APPEND _libs CURL::nettle)
endif()
if("OFF")
  find_dependency(Quiche)
  list(APPEND _libs CURL::quiche)
endif()
if("")
  find_dependency(Rustls)
  list(APPEND _libs CURL::rustls)
endif()
if("")
  find_dependency(WolfSSL)
  list(APPEND _libs CURL::wolfssl)
endif()
if("ON")
  find_dependency(Zstd)
  list(APPEND _libs CURL::zstd)
endif()

if(CMAKE_C_COMPILER_ID STREQUAL "GNU" AND WIN32 AND NOT TARGET CURL::win32_winsock)
  add_library(CURL::win32_winsock INTERFACE IMPORTED)
  set_target_properties(CURL::win32_winsock PROPERTIES INTERFACE_LINK_LIBRARIES "")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/CURLTargets.cmake")

# Alias for either shared or static library
if(NOT TARGET CURL::libcurl)
  if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.11 AND CMAKE_VERSION VERSION_LESS 3.18)
    set_target_properties(CURL::libcurl_static PROPERTIES IMPORTED_GLOBAL TRUE)
  endif()
  add_library(CURL::libcurl ALIAS CURL::libcurl_static)
endif()

if(TARGET CURL::libcurl_static)
  # CMake before CMP0099 (CMake 3.17 2020-03-20) did not propagate libdirs to
  # targets. It expected libs to have an absolute filename. As a workaround,
  # manually apply dependency libdirs, for CMake consumers without this policy.
  if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.17)
    cmake_policy(GET CMP0099 _has_CMP0099)  # https://cmake.org/cmake/help/latest/policy/CMP0099.html
  endif()
  if(NOT _has_CMP0099 AND CMAKE_VERSION VERSION_GREATER_EQUAL 3.13 AND _libs)
    set(_libdirs "")
    foreach(_lib IN LISTS _libs)
      get_target_property(_libdir "${_lib}" INTERFACE_LINK_DIRECTORIES)
      if(_libdir)
        list(APPEND _libdirs "${_libdir}")
      endif()
    endforeach()
    if(_libdirs)
      target_link_directories(CURL::libcurl_static INTERFACE ${_libdirs})
    endif()
  endif()
endif()

# For compatibility with CMake's FindCURL.cmake
set(CURL_VERSION_STRING "8.18.0-DEV")
set(CURL_LIBRARIES CURL::libcurl)
set(CURL_LIBRARIES_PRIVATE "ssh2;idn2;/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/lib/libldap.tbd;/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/lib/liblber.tbd;-framework SystemConfiguration;-framework CoreFoundation;-framework CoreServices;/opt/homebrew/Cellar/openssl@3/3.6.0/lib/libssl.dylib;/opt/homebrew/Cellar/openssl@3/3.6.0/lib/libcrypto.dylib;/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/lib/libz.tbd;brotlidec;brotlicommon;zstd;nghttp2;psl")
set_and_check(CURL_INCLUDE_DIRS "${PACKAGE_PREFIX_DIR}/include")

set(CURL_SUPPORTED_PROTOCOLS "DICT;FILE;FTP;FTPS;GOPHER;GOPHERS;HTTP;HTTPS;IMAP;IMAPS;IPFS;IPNS;LDAP;LDAPS;MQTT;POP3;POP3S;RTSP;SCP;SFTP;SMB;SMBS;SMTP;SMTPS;TELNET;TFTP;WS;WSS")
set(CURL_SUPPORTED_FEATURES "alt-svc;AsynchDNS;brotli;HSTS;HTTP2;HTTPS-proxy;IDN;IPv6;Largefile;libz;NTLM;PSL;SSL;threadsafe;TLS-SRP;UnixSockets;zstd")

foreach(_item IN LISTS CURL_SUPPORTED_PROTOCOLS CURL_SUPPORTED_FEATURES)
  set(CURL_SUPPORTS_${_item} TRUE)
endforeach()

set(_missing_req "")
foreach(_item IN LISTS CURL_FIND_COMPONENTS)
  if(CURL_SUPPORTS_${_item})
    set(CURL_${_item}_FOUND TRUE)
  elseif(CURL_FIND_REQUIRED_${_item})
    list(APPEND _missing_req ${_item})
  endif()
endforeach()

if(_missing_req)
  string(REPLACE ";" " " _missing_req "${_missing_req}")
  if(CURL_FIND_REQUIRED)
    message(FATAL_ERROR "CURL: missing required components: ${_missing_req}")
  endif()
  unset(_missing_req)
endif()

check_required_components("CURL")

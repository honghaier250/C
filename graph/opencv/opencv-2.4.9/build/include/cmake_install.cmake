# Install script for directory: /root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/include

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local/opencv")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "DEBUG")
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

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "0")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "dev")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/opencv" TYPE FILE FILES
    "/root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/include/opencv/cxeigen.hpp"
    "/root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/include/opencv/cvaux.h"
    "/root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/include/opencv/highgui.h"
    "/root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/include/opencv/cvwimage.h"
    "/root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/include/opencv/cxcore.hpp"
    "/root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/include/opencv/cv.h"
    "/root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/include/opencv/cxmisc.h"
    "/root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/include/opencv/ml.h"
    "/root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/include/opencv/cv.hpp"
    "/root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/include/opencv/cxcore.h"
    "/root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/include/opencv/cvaux.hpp"
    )
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "dev")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/opencv2" TYPE FILE FILES "/root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/include/opencv2/opencv.hpp")
endif()


CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
# Avoid warnings in higher versions
if("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" GREATER 2.6)
 CMAKE_POLICY(VERSION 2.8)
endif()

# We use PROPERTIES LINK_INTERFACE_LIBRARIES. See cmake --help-policy CMP0022
IF(CMAKE_VERSION VERSION_EQUAL "2.8.12" OR
   CMAKE_VERSION VERSION_GREATER "2.8.12")
 CMAKE_POLICY(SET CMP0022 OLD)
ENDIF()

# We use the LOCATION target property (CMP0026)
# and get_target_property() for non-existent targets (CMP0045)
# and INSTALL_NAME_DIR (CMP0042)
IF(CMAKE_VERSION VERSION_EQUAL "3.0.0" OR
   CMAKE_VERSION VERSION_GREATER "3.0.0")
 CMAKE_POLICY(SET CMP0026 OLD)
 CMAKE_POLICY(SET CMP0045 OLD)
 CMAKE_POLICY(SET CMP0042 OLD)
ENDIF()

MESSAGE(STATUS "Running cmake version ${CMAKE_VERSION}")

# Will set GIT_EXECUTABLE and GIT_FOUND
FIND_PACKAGE(Git)

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

# First, decide about build type (debug or release)
# If custom compiler flags are set or cmake is invoked with -DCMAKE_BUILD_TYPE, 
# respect user wishes and do not (re)define CMAKE_BUILD_TYPE. If WITH_DEBUG{_FULL} 
# is given, set CMAKE_BUILD_TYPE = Debug. Otherwise, use Relwithdebinfo.


IF(DEFINED CMAKE_BUILD_TYPE)
  SET(HAVE_CMAKE_BUILD_TYPE TRUE)
ENDIF()
SET(CUSTOM_C_FLAGS $ENV{CFLAGS})

# 指定生成目标
add_executable(client ${PROJECT_SOURCE_DIR}/src/client.c)
add_executable(server ${PROJECT_SOURCE_DIR}/src/server.c)

# 指定输出可执行文件路径
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

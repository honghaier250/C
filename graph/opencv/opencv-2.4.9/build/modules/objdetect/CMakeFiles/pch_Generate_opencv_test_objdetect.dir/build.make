# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.0

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list

# Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/build

# Utility rule file for pch_Generate_opencv_test_objdetect.

# Include the progress variables for this target.
include modules/objdetect/CMakeFiles/pch_Generate_opencv_test_objdetect.dir/progress.make

modules/objdetect/CMakeFiles/pch_Generate_opencv_test_objdetect: modules/objdetect/test_precomp.hpp.gch/opencv_test_objdetect_DEBUG.gch

modules/objdetect/test_precomp.hpp.gch/opencv_test_objdetect_DEBUG.gch: ../modules/objdetect/test/test_precomp.hpp
modules/objdetect/test_precomp.hpp.gch/opencv_test_objdetect_DEBUG.gch: modules/objdetect/test_precomp.hpp
modules/objdetect/test_precomp.hpp.gch/opencv_test_objdetect_DEBUG.gch: lib/libopencv_test_objdetect_pch_dephelp.a
	$(CMAKE_COMMAND) -E cmake_progress_report /root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold "Generating test_precomp.hpp.gch/opencv_test_objdetect_DEBUG.gch"
	cd /root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/build/modules/objdetect && /usr/bin/cmake -E make_directory /root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/build/modules/objdetect/test_precomp.hpp.gch
	cd /root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/build/modules/objdetect && /usr/bin/c++ -g -O0 -DDEBUG -D_DEBUG -I"/root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/modules/objdetect/test" -I"/root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/modules/features2d/include" -I"/root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/modules/highgui/include" -I"/root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/modules/imgproc/include" -I"/root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/modules/flann/include" -I"/root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/modules/core/include" -I"/root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/modules/highgui/include" -I"/root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/modules/ts/include" -I"/root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/modules/objdetect/include" -I"/root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/modules/highgui/include" -I"/root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/modules/imgproc/include" -I"/root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/modules/core/include" -isystem"/root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/build/modules/objdetect" -I"/root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/modules/objdetect/src" -I"/root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/modules/objdetect/include" -I"/root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/modules/highgui/include" -I"/root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/modules/imgproc/include" -I"/root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/modules/core/include" -isystem"/root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/build/modules/objdetect" -I"/root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/modules/objdetect/src" -I"/root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/modules/objdetect/include" -isystem"/root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/build" -fsigned-char -W -Wall -Werror=return-type -Werror=address -Werror=sequence-point -Wformat -Werror=format-security -Wmissing-declarations -Wmissing-prototypes -Wstrict-prototypes -Wundef -Winit-self -Wpointer-arith -Wshadow -Wsign-promo -Wno-narrowing -Wno-delete-non-virtual-dtor -Wno-unnamed-type-template-args -fdiagnostics-show-option -Wno-long-long -pthread -fomit-frame-pointer -msse -msse2 -msse3 -ffunction-sections -x c++-header -o /root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/build/modules/objdetect/test_precomp.hpp.gch/opencv_test_objdetect_DEBUG.gch /root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/build/modules/objdetect/test_precomp.hpp

modules/objdetect/test_precomp.hpp: ../modules/objdetect/test/test_precomp.hpp
	$(CMAKE_COMMAND) -E cmake_progress_report /root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/build/CMakeFiles $(CMAKE_PROGRESS_2)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold "Generating test_precomp.hpp"
	cd /root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/build/modules/objdetect && /usr/bin/cmake -E copy /root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/modules/objdetect/test/test_precomp.hpp /root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/build/modules/objdetect/test_precomp.hpp

pch_Generate_opencv_test_objdetect: modules/objdetect/CMakeFiles/pch_Generate_opencv_test_objdetect
pch_Generate_opencv_test_objdetect: modules/objdetect/test_precomp.hpp.gch/opencv_test_objdetect_DEBUG.gch
pch_Generate_opencv_test_objdetect: modules/objdetect/test_precomp.hpp
pch_Generate_opencv_test_objdetect: modules/objdetect/CMakeFiles/pch_Generate_opencv_test_objdetect.dir/build.make
.PHONY : pch_Generate_opencv_test_objdetect

# Rule to build all files generated by this target.
modules/objdetect/CMakeFiles/pch_Generate_opencv_test_objdetect.dir/build: pch_Generate_opencv_test_objdetect
.PHONY : modules/objdetect/CMakeFiles/pch_Generate_opencv_test_objdetect.dir/build

modules/objdetect/CMakeFiles/pch_Generate_opencv_test_objdetect.dir/clean:
	cd /root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/build/modules/objdetect && $(CMAKE_COMMAND) -P CMakeFiles/pch_Generate_opencv_test_objdetect.dir/cmake_clean.cmake
.PHONY : modules/objdetect/CMakeFiles/pch_Generate_opencv_test_objdetect.dir/clean

modules/objdetect/CMakeFiles/pch_Generate_opencv_test_objdetect.dir/depend:
	cd /root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9 /root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/modules/objdetect /root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/build /root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/build/modules/objdetect /root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/build/modules/objdetect/CMakeFiles/pch_Generate_opencv_test_objdetect.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : modules/objdetect/CMakeFiles/pch_Generate_opencv_test_objdetect.dir/depend


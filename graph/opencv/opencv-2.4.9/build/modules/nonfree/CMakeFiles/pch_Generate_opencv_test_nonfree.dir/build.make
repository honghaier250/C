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

# Utility rule file for pch_Generate_opencv_test_nonfree.

# Include the progress variables for this target.
include modules/nonfree/CMakeFiles/pch_Generate_opencv_test_nonfree.dir/progress.make

modules/nonfree/CMakeFiles/pch_Generate_opencv_test_nonfree: modules/nonfree/test_precomp.hpp.gch/opencv_test_nonfree_DEBUG.gch

modules/nonfree/test_precomp.hpp.gch/opencv_test_nonfree_DEBUG.gch: ../modules/nonfree/test/test_precomp.hpp
modules/nonfree/test_precomp.hpp.gch/opencv_test_nonfree_DEBUG.gch: modules/nonfree/test_precomp.hpp
modules/nonfree/test_precomp.hpp.gch/opencv_test_nonfree_DEBUG.gch: lib/libopencv_test_nonfree_pch_dephelp.a
	$(CMAKE_COMMAND) -E cmake_progress_report /root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold "Generating test_precomp.hpp.gch/opencv_test_nonfree_DEBUG.gch"
	cd /root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/build/modules/nonfree && /usr/bin/cmake -E make_directory /root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/build/modules/nonfree/test_precomp.hpp.gch
	cd /root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/build/modules/nonfree && /usr/bin/c++ -g -O0 -DDEBUG -D_DEBUG -I"/root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/modules/nonfree/test" -I"/root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/modules/features2d/include" -I"/root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/modules/highgui/include" -I"/root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/modules/imgproc/include" -I"/root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/modules/flann/include" -I"/root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/modules/core/include" -I"/root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/modules/highgui/include" -I"/root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/modules/ts/include" -I"/root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/modules/nonfree/include" -I"/root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/modules/ocl/include" -I"/root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/modules/gpu/include" -I"/root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/modules/photo/include" -I"/root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/modules/objdetect/include" -I"/root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/modules/legacy/include" -I"/root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/modules/video/include" -I"/root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/modules/ml/include" -I"/root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/modules/calib3d/include" -I"/root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/modules/features2d/include" -I"/root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/modules/highgui/include" -I"/root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/modules/imgproc/include" -I"/root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/modules/flann/include" -I"/root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/modules/core/include" -isystem"/root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/build/modules/nonfree" -I"/root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/modules/nonfree/src" -I"/root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/modules/nonfree/include" -isystem"/root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/3rdparty/include/opencl/1.2" -I"/root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/modules/ocl/include" -I"/root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/modules/gpu/include" -I"/root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/modules/photo/include" -I"/root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/modules/objdetect/include" -I"/root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/modules/legacy/include" -I"/root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/modules/video/include" -I"/root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/modules/ml/include" -I"/root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/modules/calib3d/include" -I"/root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/modules/features2d/include" -I"/root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/modules/highgui/include" -I"/root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/modules/imgproc/include" -I"/root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/modules/flann/include" -I"/root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/modules/core/include" -isystem"/root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/build/modules/nonfree" -I"/root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/modules/nonfree/src" -I"/root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/modules/nonfree/include" -isystem"/root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/build" -fsigned-char -W -Wall -Werror=return-type -Werror=address -Werror=sequence-point -Wformat -Werror=format-security -Wmissing-declarations -Wmissing-prototypes -Wstrict-prototypes -Winit-self -Wpointer-arith -Wsign-promo -Wno-narrowing -Wno-delete-non-virtual-dtor -Wno-unnamed-type-template-args -fdiagnostics-show-option -Wno-long-long -pthread -fomit-frame-pointer -msse -msse2 -msse3 -ffunction-sections -Wno-undef -Wno-shadow -x c++-header -o /root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/build/modules/nonfree/test_precomp.hpp.gch/opencv_test_nonfree_DEBUG.gch /root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/build/modules/nonfree/test_precomp.hpp

modules/nonfree/test_precomp.hpp: ../modules/nonfree/test/test_precomp.hpp
	$(CMAKE_COMMAND) -E cmake_progress_report /root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/build/CMakeFiles $(CMAKE_PROGRESS_2)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold "Generating test_precomp.hpp"
	cd /root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/build/modules/nonfree && /usr/bin/cmake -E copy /root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/modules/nonfree/test/test_precomp.hpp /root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/build/modules/nonfree/test_precomp.hpp

pch_Generate_opencv_test_nonfree: modules/nonfree/CMakeFiles/pch_Generate_opencv_test_nonfree
pch_Generate_opencv_test_nonfree: modules/nonfree/test_precomp.hpp.gch/opencv_test_nonfree_DEBUG.gch
pch_Generate_opencv_test_nonfree: modules/nonfree/test_precomp.hpp
pch_Generate_opencv_test_nonfree: modules/nonfree/CMakeFiles/pch_Generate_opencv_test_nonfree.dir/build.make
.PHONY : pch_Generate_opencv_test_nonfree

# Rule to build all files generated by this target.
modules/nonfree/CMakeFiles/pch_Generate_opencv_test_nonfree.dir/build: pch_Generate_opencv_test_nonfree
.PHONY : modules/nonfree/CMakeFiles/pch_Generate_opencv_test_nonfree.dir/build

modules/nonfree/CMakeFiles/pch_Generate_opencv_test_nonfree.dir/clean:
	cd /root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/build/modules/nonfree && $(CMAKE_COMMAND) -P CMakeFiles/pch_Generate_opencv_test_nonfree.dir/cmake_clean.cmake
.PHONY : modules/nonfree/CMakeFiles/pch_Generate_opencv_test_nonfree.dir/clean

modules/nonfree/CMakeFiles/pch_Generate_opencv_test_nonfree.dir/depend:
	cd /root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9 /root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/modules/nonfree /root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/build /root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/build/modules/nonfree /root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/build/modules/nonfree/CMakeFiles/pch_Generate_opencv_test_nonfree.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : modules/nonfree/CMakeFiles/pch_Generate_opencv_test_nonfree.dir/depend


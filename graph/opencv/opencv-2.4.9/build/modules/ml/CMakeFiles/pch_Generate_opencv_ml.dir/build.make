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

# Utility rule file for pch_Generate_opencv_ml.

# Include the progress variables for this target.
include modules/ml/CMakeFiles/pch_Generate_opencv_ml.dir/progress.make

modules/ml/CMakeFiles/pch_Generate_opencv_ml: modules/ml/precomp.hpp.gch/opencv_ml_DEBUG.gch

modules/ml/precomp.hpp.gch/opencv_ml_DEBUG.gch: ../modules/ml/src/precomp.hpp
modules/ml/precomp.hpp.gch/opencv_ml_DEBUG.gch: modules/ml/precomp.hpp
modules/ml/precomp.hpp.gch/opencv_ml_DEBUG.gch: lib/libopencv_ml_pch_dephelp.a
	$(CMAKE_COMMAND) -E cmake_progress_report /root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold "Generating precomp.hpp.gch/opencv_ml_DEBUG.gch"
	cd /root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/build/modules/ml && /usr/bin/cmake -E make_directory /root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/build/modules/ml/precomp.hpp.gch
	cd /root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/build/modules/ml && /usr/bin/c++ -g -O0 -DDEBUG -D_DEBUG -fPIC -I"/root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/modules/core/include" -isystem"/root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/build/modules/ml" -I"/root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/modules/ml/src" -I"/root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/modules/ml/include" -isystem"/root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/build" -fsigned-char -W -Wall -Werror=return-type -Werror=address -Werror=sequence-point -Wformat -Werror=format-security -Wmissing-declarations -Wmissing-prototypes -Wstrict-prototypes -Wundef -Winit-self -Wpointer-arith -Wshadow -Wsign-promo -Wno-narrowing -Wno-delete-non-virtual-dtor -Wno-unnamed-type-template-args -fdiagnostics-show-option -Wno-long-long -pthread -fomit-frame-pointer -msse -msse2 -msse3 -ffunction-sections -DCVAPI_EXPORTS -x c++-header -o /root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/build/modules/ml/precomp.hpp.gch/opencv_ml_DEBUG.gch /root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/build/modules/ml/precomp.hpp

modules/ml/precomp.hpp: ../modules/ml/src/precomp.hpp
	$(CMAKE_COMMAND) -E cmake_progress_report /root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/build/CMakeFiles $(CMAKE_PROGRESS_2)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold "Generating precomp.hpp"
	cd /root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/build/modules/ml && /usr/bin/cmake -E copy /root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/modules/ml/src/precomp.hpp /root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/build/modules/ml/precomp.hpp

pch_Generate_opencv_ml: modules/ml/CMakeFiles/pch_Generate_opencv_ml
pch_Generate_opencv_ml: modules/ml/precomp.hpp.gch/opencv_ml_DEBUG.gch
pch_Generate_opencv_ml: modules/ml/precomp.hpp
pch_Generate_opencv_ml: modules/ml/CMakeFiles/pch_Generate_opencv_ml.dir/build.make
.PHONY : pch_Generate_opencv_ml

# Rule to build all files generated by this target.
modules/ml/CMakeFiles/pch_Generate_opencv_ml.dir/build: pch_Generate_opencv_ml
.PHONY : modules/ml/CMakeFiles/pch_Generate_opencv_ml.dir/build

modules/ml/CMakeFiles/pch_Generate_opencv_ml.dir/clean:
	cd /root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/build/modules/ml && $(CMAKE_COMMAND) -P CMakeFiles/pch_Generate_opencv_ml.dir/cmake_clean.cmake
.PHONY : modules/ml/CMakeFiles/pch_Generate_opencv_ml.dir/clean

modules/ml/CMakeFiles/pch_Generate_opencv_ml.dir/depend:
	cd /root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9 /root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/modules/ml /root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/build /root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/build/modules/ml /root/workspace/exercise/languages/C/graph/opencv/opencv-2.4.9/build/modules/ml/CMakeFiles/pch_Generate_opencv_ml.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : modules/ml/CMakeFiles/pch_Generate_opencv_ml.dir/depend


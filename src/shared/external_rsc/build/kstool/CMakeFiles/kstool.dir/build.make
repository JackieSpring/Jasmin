# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.23

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /opt/homebrew/Cellar/cmake/3.23.2/bin/cmake

# The command to remove a file.
RM = /opt/homebrew/Cellar/cmake/3.23.2/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/camola/Desktop/università/asm_interpreter/jasmin/src/shared/external_rsc/keystone-master

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/camola/Desktop/università/asm_interpreter/jasmin/src/shared/external_rsc/build

# Include any dependencies generated for this target.
include kstool/CMakeFiles/kstool.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include kstool/CMakeFiles/kstool.dir/compiler_depend.make

# Include the progress variables for this target.
include kstool/CMakeFiles/kstool.dir/progress.make

# Include the compile flags for this target's objects.
include kstool/CMakeFiles/kstool.dir/flags.make

kstool/CMakeFiles/kstool.dir/kstool.cpp.o: kstool/CMakeFiles/kstool.dir/flags.make
kstool/CMakeFiles/kstool.dir/kstool.cpp.o: /Users/camola/Desktop/università/asm_interpreter/jasmin/src/shared/external_rsc/keystone-master/kstool/kstool.cpp
kstool/CMakeFiles/kstool.dir/kstool.cpp.o: kstool/CMakeFiles/kstool.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/camola/Desktop/università/asm_interpreter/jasmin/src/shared/external_rsc/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object kstool/CMakeFiles/kstool.dir/kstool.cpp.o"
	cd /Users/camola/Desktop/università/asm_interpreter/jasmin/src/shared/external_rsc/build/kstool && /Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT kstool/CMakeFiles/kstool.dir/kstool.cpp.o -MF CMakeFiles/kstool.dir/kstool.cpp.o.d -o CMakeFiles/kstool.dir/kstool.cpp.o -c /Users/camola/Desktop/università/asm_interpreter/jasmin/src/shared/external_rsc/keystone-master/kstool/kstool.cpp

kstool/CMakeFiles/kstool.dir/kstool.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/kstool.dir/kstool.cpp.i"
	cd /Users/camola/Desktop/università/asm_interpreter/jasmin/src/shared/external_rsc/build/kstool && /Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/camola/Desktop/università/asm_interpreter/jasmin/src/shared/external_rsc/keystone-master/kstool/kstool.cpp > CMakeFiles/kstool.dir/kstool.cpp.i

kstool/CMakeFiles/kstool.dir/kstool.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/kstool.dir/kstool.cpp.s"
	cd /Users/camola/Desktop/università/asm_interpreter/jasmin/src/shared/external_rsc/build/kstool && /Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/camola/Desktop/università/asm_interpreter/jasmin/src/shared/external_rsc/keystone-master/kstool/kstool.cpp -o CMakeFiles/kstool.dir/kstool.cpp.s

# Object files for target kstool
kstool_OBJECTS = \
"CMakeFiles/kstool.dir/kstool.cpp.o"

# External object files for target kstool
kstool_EXTERNAL_OBJECTS =

kstool/kstool: kstool/CMakeFiles/kstool.dir/kstool.cpp.o
kstool/kstool: kstool/CMakeFiles/kstool.dir/build.make
kstool/kstool: llvm/lib/libkeystone.a
kstool/kstool: kstool/CMakeFiles/kstool.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/camola/Desktop/università/asm_interpreter/jasmin/src/shared/external_rsc/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable kstool"
	cd /Users/camola/Desktop/università/asm_interpreter/jasmin/src/shared/external_rsc/build/kstool && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/kstool.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
kstool/CMakeFiles/kstool.dir/build: kstool/kstool
.PHONY : kstool/CMakeFiles/kstool.dir/build

kstool/CMakeFiles/kstool.dir/clean:
	cd /Users/camola/Desktop/università/asm_interpreter/jasmin/src/shared/external_rsc/build/kstool && $(CMAKE_COMMAND) -P CMakeFiles/kstool.dir/cmake_clean.cmake
.PHONY : kstool/CMakeFiles/kstool.dir/clean

kstool/CMakeFiles/kstool.dir/depend:
	cd /Users/camola/Desktop/università/asm_interpreter/jasmin/src/shared/external_rsc/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/camola/Desktop/università/asm_interpreter/jasmin/src/shared/external_rsc/keystone-master /Users/camola/Desktop/università/asm_interpreter/jasmin/src/shared/external_rsc/keystone-master/kstool /Users/camola/Desktop/università/asm_interpreter/jasmin/src/shared/external_rsc/build /Users/camola/Desktop/università/asm_interpreter/jasmin/src/shared/external_rsc/build/kstool /Users/camola/Desktop/università/asm_interpreter/jasmin/src/shared/external_rsc/build/kstool/CMakeFiles/kstool.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : kstool/CMakeFiles/kstool.dir/depend


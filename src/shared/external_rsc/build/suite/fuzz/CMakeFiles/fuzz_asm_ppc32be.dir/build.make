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
include suite/fuzz/CMakeFiles/fuzz_asm_ppc32be.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include suite/fuzz/CMakeFiles/fuzz_asm_ppc32be.dir/compiler_depend.make

# Include the progress variables for this target.
include suite/fuzz/CMakeFiles/fuzz_asm_ppc32be.dir/progress.make

# Include the compile flags for this target's objects.
include suite/fuzz/CMakeFiles/fuzz_asm_ppc32be.dir/flags.make

suite/fuzz/CMakeFiles/fuzz_asm_ppc32be.dir/onefile.c.o: suite/fuzz/CMakeFiles/fuzz_asm_ppc32be.dir/flags.make
suite/fuzz/CMakeFiles/fuzz_asm_ppc32be.dir/onefile.c.o: /Users/camola/Desktop/università/asm_interpreter/jasmin/src/shared/external_rsc/keystone-master/suite/fuzz/onefile.c
suite/fuzz/CMakeFiles/fuzz_asm_ppc32be.dir/onefile.c.o: suite/fuzz/CMakeFiles/fuzz_asm_ppc32be.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/camola/Desktop/università/asm_interpreter/jasmin/src/shared/external_rsc/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object suite/fuzz/CMakeFiles/fuzz_asm_ppc32be.dir/onefile.c.o"
	cd /Users/camola/Desktop/università/asm_interpreter/jasmin/src/shared/external_rsc/build/suite/fuzz && /Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT suite/fuzz/CMakeFiles/fuzz_asm_ppc32be.dir/onefile.c.o -MF CMakeFiles/fuzz_asm_ppc32be.dir/onefile.c.o.d -o CMakeFiles/fuzz_asm_ppc32be.dir/onefile.c.o -c /Users/camola/Desktop/università/asm_interpreter/jasmin/src/shared/external_rsc/keystone-master/suite/fuzz/onefile.c

suite/fuzz/CMakeFiles/fuzz_asm_ppc32be.dir/onefile.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/fuzz_asm_ppc32be.dir/onefile.c.i"
	cd /Users/camola/Desktop/università/asm_interpreter/jasmin/src/shared/external_rsc/build/suite/fuzz && /Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/camola/Desktop/università/asm_interpreter/jasmin/src/shared/external_rsc/keystone-master/suite/fuzz/onefile.c > CMakeFiles/fuzz_asm_ppc32be.dir/onefile.c.i

suite/fuzz/CMakeFiles/fuzz_asm_ppc32be.dir/onefile.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/fuzz_asm_ppc32be.dir/onefile.c.s"
	cd /Users/camola/Desktop/università/asm_interpreter/jasmin/src/shared/external_rsc/build/suite/fuzz && /Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/camola/Desktop/università/asm_interpreter/jasmin/src/shared/external_rsc/keystone-master/suite/fuzz/onefile.c -o CMakeFiles/fuzz_asm_ppc32be.dir/onefile.c.s

suite/fuzz/CMakeFiles/fuzz_asm_ppc32be.dir/fuzz_asm_ppc32be.c.o: suite/fuzz/CMakeFiles/fuzz_asm_ppc32be.dir/flags.make
suite/fuzz/CMakeFiles/fuzz_asm_ppc32be.dir/fuzz_asm_ppc32be.c.o: /Users/camola/Desktop/università/asm_interpreter/jasmin/src/shared/external_rsc/keystone-master/suite/fuzz/fuzz_asm_ppc32be.c
suite/fuzz/CMakeFiles/fuzz_asm_ppc32be.dir/fuzz_asm_ppc32be.c.o: suite/fuzz/CMakeFiles/fuzz_asm_ppc32be.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/camola/Desktop/università/asm_interpreter/jasmin/src/shared/external_rsc/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object suite/fuzz/CMakeFiles/fuzz_asm_ppc32be.dir/fuzz_asm_ppc32be.c.o"
	cd /Users/camola/Desktop/università/asm_interpreter/jasmin/src/shared/external_rsc/build/suite/fuzz && /Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT suite/fuzz/CMakeFiles/fuzz_asm_ppc32be.dir/fuzz_asm_ppc32be.c.o -MF CMakeFiles/fuzz_asm_ppc32be.dir/fuzz_asm_ppc32be.c.o.d -o CMakeFiles/fuzz_asm_ppc32be.dir/fuzz_asm_ppc32be.c.o -c /Users/camola/Desktop/università/asm_interpreter/jasmin/src/shared/external_rsc/keystone-master/suite/fuzz/fuzz_asm_ppc32be.c

suite/fuzz/CMakeFiles/fuzz_asm_ppc32be.dir/fuzz_asm_ppc32be.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/fuzz_asm_ppc32be.dir/fuzz_asm_ppc32be.c.i"
	cd /Users/camola/Desktop/università/asm_interpreter/jasmin/src/shared/external_rsc/build/suite/fuzz && /Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/camola/Desktop/università/asm_interpreter/jasmin/src/shared/external_rsc/keystone-master/suite/fuzz/fuzz_asm_ppc32be.c > CMakeFiles/fuzz_asm_ppc32be.dir/fuzz_asm_ppc32be.c.i

suite/fuzz/CMakeFiles/fuzz_asm_ppc32be.dir/fuzz_asm_ppc32be.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/fuzz_asm_ppc32be.dir/fuzz_asm_ppc32be.c.s"
	cd /Users/camola/Desktop/università/asm_interpreter/jasmin/src/shared/external_rsc/build/suite/fuzz && /Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/camola/Desktop/università/asm_interpreter/jasmin/src/shared/external_rsc/keystone-master/suite/fuzz/fuzz_asm_ppc32be.c -o CMakeFiles/fuzz_asm_ppc32be.dir/fuzz_asm_ppc32be.c.s

# Object files for target fuzz_asm_ppc32be
fuzz_asm_ppc32be_OBJECTS = \
"CMakeFiles/fuzz_asm_ppc32be.dir/onefile.c.o" \
"CMakeFiles/fuzz_asm_ppc32be.dir/fuzz_asm_ppc32be.c.o"

# External object files for target fuzz_asm_ppc32be
fuzz_asm_ppc32be_EXTERNAL_OBJECTS =

suite/fuzz/fuzz_asm_ppc32be: suite/fuzz/CMakeFiles/fuzz_asm_ppc32be.dir/onefile.c.o
suite/fuzz/fuzz_asm_ppc32be: suite/fuzz/CMakeFiles/fuzz_asm_ppc32be.dir/fuzz_asm_ppc32be.c.o
suite/fuzz/fuzz_asm_ppc32be: suite/fuzz/CMakeFiles/fuzz_asm_ppc32be.dir/build.make
suite/fuzz/fuzz_asm_ppc32be: llvm/lib/libkeystone.a
suite/fuzz/fuzz_asm_ppc32be: suite/fuzz/CMakeFiles/fuzz_asm_ppc32be.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/camola/Desktop/università/asm_interpreter/jasmin/src/shared/external_rsc/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable fuzz_asm_ppc32be"
	cd /Users/camola/Desktop/università/asm_interpreter/jasmin/src/shared/external_rsc/build/suite/fuzz && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/fuzz_asm_ppc32be.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
suite/fuzz/CMakeFiles/fuzz_asm_ppc32be.dir/build: suite/fuzz/fuzz_asm_ppc32be
.PHONY : suite/fuzz/CMakeFiles/fuzz_asm_ppc32be.dir/build

suite/fuzz/CMakeFiles/fuzz_asm_ppc32be.dir/clean:
	cd /Users/camola/Desktop/università/asm_interpreter/jasmin/src/shared/external_rsc/build/suite/fuzz && $(CMAKE_COMMAND) -P CMakeFiles/fuzz_asm_ppc32be.dir/cmake_clean.cmake
.PHONY : suite/fuzz/CMakeFiles/fuzz_asm_ppc32be.dir/clean

suite/fuzz/CMakeFiles/fuzz_asm_ppc32be.dir/depend:
	cd /Users/camola/Desktop/università/asm_interpreter/jasmin/src/shared/external_rsc/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/camola/Desktop/università/asm_interpreter/jasmin/src/shared/external_rsc/keystone-master /Users/camola/Desktop/università/asm_interpreter/jasmin/src/shared/external_rsc/keystone-master/suite/fuzz /Users/camola/Desktop/università/asm_interpreter/jasmin/src/shared/external_rsc/build /Users/camola/Desktop/università/asm_interpreter/jasmin/src/shared/external_rsc/build/suite/fuzz /Users/camola/Desktop/università/asm_interpreter/jasmin/src/shared/external_rsc/build/suite/fuzz/CMakeFiles/fuzz_asm_ppc32be.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : suite/fuzz/CMakeFiles/fuzz_asm_ppc32be.dir/depend


# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


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
CMAKE_SOURCE_DIR = /home/xiongbaohua/source/anet

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/xiongbaohua/source/anet

# Include any dependencies generated for this target.
include CMakeFiles/tcpServer.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/tcpServer.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/tcpServer.dir/flags.make

CMakeFiles/tcpServer.dir/anet.cpp.o: CMakeFiles/tcpServer.dir/flags.make
CMakeFiles/tcpServer.dir/anet.cpp.o: anet.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/xiongbaohua/source/anet/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/tcpServer.dir/anet.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/tcpServer.dir/anet.cpp.o -c /home/xiongbaohua/source/anet/anet.cpp

CMakeFiles/tcpServer.dir/anet.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/tcpServer.dir/anet.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/xiongbaohua/source/anet/anet.cpp > CMakeFiles/tcpServer.dir/anet.cpp.i

CMakeFiles/tcpServer.dir/anet.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/tcpServer.dir/anet.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/xiongbaohua/source/anet/anet.cpp -o CMakeFiles/tcpServer.dir/anet.cpp.s

CMakeFiles/tcpServer.dir/tcpServer.cpp.o: CMakeFiles/tcpServer.dir/flags.make
CMakeFiles/tcpServer.dir/tcpServer.cpp.o: tcpServer.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/xiongbaohua/source/anet/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/tcpServer.dir/tcpServer.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/tcpServer.dir/tcpServer.cpp.o -c /home/xiongbaohua/source/anet/tcpServer.cpp

CMakeFiles/tcpServer.dir/tcpServer.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/tcpServer.dir/tcpServer.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/xiongbaohua/source/anet/tcpServer.cpp > CMakeFiles/tcpServer.dir/tcpServer.cpp.i

CMakeFiles/tcpServer.dir/tcpServer.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/tcpServer.dir/tcpServer.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/xiongbaohua/source/anet/tcpServer.cpp -o CMakeFiles/tcpServer.dir/tcpServer.cpp.s

# Object files for target tcpServer
tcpServer_OBJECTS = \
"CMakeFiles/tcpServer.dir/anet.cpp.o" \
"CMakeFiles/tcpServer.dir/tcpServer.cpp.o"

# External object files for target tcpServer
tcpServer_EXTERNAL_OBJECTS =

tcpServer: CMakeFiles/tcpServer.dir/anet.cpp.o
tcpServer: CMakeFiles/tcpServer.dir/tcpServer.cpp.o
tcpServer: CMakeFiles/tcpServer.dir/build.make
tcpServer: CMakeFiles/tcpServer.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/xiongbaohua/source/anet/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable tcpServer"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/tcpServer.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/tcpServer.dir/build: tcpServer

.PHONY : CMakeFiles/tcpServer.dir/build

CMakeFiles/tcpServer.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/tcpServer.dir/cmake_clean.cmake
.PHONY : CMakeFiles/tcpServer.dir/clean

CMakeFiles/tcpServer.dir/depend:
	cd /home/xiongbaohua/source/anet && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/xiongbaohua/source/anet /home/xiongbaohua/source/anet /home/xiongbaohua/source/anet /home/xiongbaohua/source/anet /home/xiongbaohua/source/anet/CMakeFiles/tcpServer.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/tcpServer.dir/depend


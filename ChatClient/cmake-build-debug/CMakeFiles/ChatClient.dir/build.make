# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.17

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
CMAKE_COMMAND = /snap/clion/137/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /snap/clion/137/bin/cmake/linux/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/ivan/CLionProjects/ChatClient

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/ivan/CLionProjects/ChatClient/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/ChatClient.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/ChatClient.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/ChatClient.dir/flags.make

CMakeFiles/ChatClient.dir/client.cpp.o: CMakeFiles/ChatClient.dir/flags.make
CMakeFiles/ChatClient.dir/client.cpp.o: ../client.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ivan/CLionProjects/ChatClient/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/ChatClient.dir/client.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/ChatClient.dir/client.cpp.o -c /home/ivan/CLionProjects/ChatClient/client.cpp

CMakeFiles/ChatClient.dir/client.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/ChatClient.dir/client.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ivan/CLionProjects/ChatClient/client.cpp > CMakeFiles/ChatClient.dir/client.cpp.i

CMakeFiles/ChatClient.dir/client.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/ChatClient.dir/client.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ivan/CLionProjects/ChatClient/client.cpp -o CMakeFiles/ChatClient.dir/client.cpp.s

# Object files for target ChatClient
ChatClient_OBJECTS = \
"CMakeFiles/ChatClient.dir/client.cpp.o"

# External object files for target ChatClient
ChatClient_EXTERNAL_OBJECTS =

ChatClient: CMakeFiles/ChatClient.dir/client.cpp.o
ChatClient: CMakeFiles/ChatClient.dir/build.make
ChatClient: CMakeFiles/ChatClient.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/ivan/CLionProjects/ChatClient/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ChatClient"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/ChatClient.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/ChatClient.dir/build: ChatClient

.PHONY : CMakeFiles/ChatClient.dir/build

CMakeFiles/ChatClient.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/ChatClient.dir/cmake_clean.cmake
.PHONY : CMakeFiles/ChatClient.dir/clean

CMakeFiles/ChatClient.dir/depend:
	cd /home/ivan/CLionProjects/ChatClient/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/ivan/CLionProjects/ChatClient /home/ivan/CLionProjects/ChatClient /home/ivan/CLionProjects/ChatClient/cmake-build-debug /home/ivan/CLionProjects/ChatClient/cmake-build-debug /home/ivan/CLionProjects/ChatClient/cmake-build-debug/CMakeFiles/ChatClient.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/ChatClient.dir/depend


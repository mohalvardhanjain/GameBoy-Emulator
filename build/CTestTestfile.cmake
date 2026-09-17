# CMake generated Testfile for 
# Source directory: /home/mohalVardhanJemimah/Documents/projects/Game-Boy-Emulator
# Build directory: /home/mohalVardhanJemimah/Documents/projects/Game-Boy-Emulator/build
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(check_gbe "/home/mohalVardhanJemimah/Documents/projects/Game-Boy-Emulator/build/tests/check_gbe")
set_tests_properties(check_gbe PROPERTIES  _BACKTRACE_TRIPLES "/home/mohalVardhanJemimah/Documents/projects/Game-Boy-Emulator/CMakeLists.txt;138;add_test;/home/mohalVardhanJemimah/Documents/projects/Game-Boy-Emulator/CMakeLists.txt;0;")
subdirs("lib")
subdirs("gbemu")
subdirs("tests")

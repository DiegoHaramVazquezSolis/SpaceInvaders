# CMake generated Testfile for 
# Source directory: C:/Users/Edson Montelongo/Documents/GitHub/SpaceInvaders/libs/raylib/src
# Build directory: C:/Users/Edson Montelongo/Documents/GitHub/SpaceInvaders/cmake-build-debug/libs/raylib/src
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(pkg-config--static "C:/Users/Edson Montelongo/Documents/GitHub/SpaceInvaders/libs/raylib/src/../cmake/test-pkgconfig.sh" "--static")
set_tests_properties(pkg-config--static PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/Edson Montelongo/Documents/GitHub/SpaceInvaders/libs/raylib/src/CMakeLists.txt;212;add_test;C:/Users/Edson Montelongo/Documents/GitHub/SpaceInvaders/libs/raylib/src/CMakeLists.txt;0;")
subdirs("external/glfw")

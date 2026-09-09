cmake_minimum_required(VERSION 3.20)
project(ArchLintCpp VERSION 0.1.1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Must be included before find_package(LLVM): LLVM's bundled Findzstd.cmake
# references CMAKE_INSTALL_LIBDIR, and errors on newer CMake if it is unset
# (https://github.com/llvm/llvm-project/issues/83802).
include(GNUInstallDirs)

find_package(LLVM REQUIRED CONFIG)
find_package(Clang REQUIRED CONFIG)

if(NOT LLVM_PACKAGE_VERSION VERSION_GREATER_EQUAL 18 OR LLVM_PACKAGE_VERSION VERSION_GREATER_EQUAL 19)
    message(FATAL_ERROR "ArchLintCpp currently requires LLVM/Clang 18.x; found ${LLVM_PACKAGE_VERSION}")
endif()

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

add_library(archlint_core
    src/Config.cpp
    src/RuleEngine.cpp
    src/ClangDependencyExtractor.cpp
)

target_include_directories(archlint_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${LLVM_INCLUDE_DIRS}
        ${CLANG_INCLUDE_DIRS}
)

target_compile_definitions(archlint_core PRIVATE ${LLVM_DEFINITIONS})

target_link_libraries(archlint_core
    PUBLIC
        clangAST
        clangBasic
        clangFrontend
        clangLex
        clangTooling
        LLVMCore
        LLVMSupport
)

add_executable(archlint-cpp src/main.cpp)
target_link_libraries(archlint-cpp PRIVATE archlint_core)

include(CTest)
if(BUILD_TESTING)
    add_executable(archlint-tests tests/RuleEngineTests.cpp)
    target_link_libraries(archlint-tests PRIVATE archlint_core)
    add_test(NAME archlint-tests COMMAND archlint-tests)
endif()

install(TARGETS archlint-cpp RUNTIME DESTINATION bin)

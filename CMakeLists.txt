cmake_minimum_required(VERSION 3.15)
project(clangPlugins)

find_package(LLVM 10 REQUIRED CONFIG)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(LT_LLVM_INSTALL_DIR "" CACHE PATH "LLVM installation directory")

set(LT_LLVM_INCLUDE_DIR "${LT_LLVM_INSTALL_DIR}/include/llvm")
if(NOT EXISTS "${LT_LLVM_INCLUDE_DIR}")
message(FATAL_ERROR
  " LT_LLVM_INSTALL_DIR (${LT_LLVM_INCLUDE_DIR}) is invalid.")
endif()

# 1.2 Check that the LLVMConfig.cmake file exists (the location depends on the
# OS)
set(LT_VALID_INSTALLATION FALSE)

# Ubuntu + Darwin
if(EXISTS "${LT_LLVM_INSTALL_DIR}/lib/cmake/llvm/LLVMConfig.cmake")
  set(LT_VALID_INSTALLATION TRUE)
endif()

# Fedora
if(EXISTS "${LT_LLVM_INSTALL_DIR}/lib64/cmake/llvm/LLVMConfig.cmake")
  set(LT_VALID_INSTALLATION TRUE)
endif()

if(NOT ${LT_VALID_INSTALLATION})
  message(FATAL_ERROR
    "LLVM installation directory, (${LT_LLVM_INSTALL_DIR}), is invalid. Couldn't
    find LLVMConfig.cmake.")
endif()

# LLVM is normally built without RTTI. Be consistent with that.
if(NOT LLVM_ENABLE_RTTI)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
endif()

include_directories(SYSTEM ${LLVM_INCLUDE_DIRS})

add_library(ViewIf MODULE ViewIf.cpp)
add_library(FindIfBoundary MODULE FindIfBoundary.cpp)

target_link_libraries(FindIfBoundary PRIVATE
    clangAST
    clangASTMatchers
    clangBasic
    clangFrontend
    clangTooling
)

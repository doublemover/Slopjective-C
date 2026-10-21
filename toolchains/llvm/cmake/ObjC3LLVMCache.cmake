# Shared reproducible cache settings for the pinned LLVM foundation.
# Pass -G Ninja on the CMake command line; CMAKE_GENERATOR cannot be selected
# from an initial cache file.
set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
set(LLVM_ENABLE_ASSERTIONS ON CACHE BOOL "" FORCE)
set(LLVM_ENABLE_RTTI ON CACHE BOOL "" FORCE)
set(LLVM_ENABLE_EH ON CACHE BOOL "" FORCE)
set(LLVM_TARGETS_TO_BUILD "X86" CACHE STRING "" FORCE)
set(LLVM_ENABLE_PROJECTS "clang;lld;clang-tools-extra;lldb" CACHE STRING "" FORCE)
set(LLVM_ENABLE_RUNTIMES "" CACHE STRING "" FORCE)
set(LLVM_ENABLE_BOOTSTRAP OFF CACHE BOOL "" FORCE)
if(CMAKE_HOST_WIN32)
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL" CACHE STRING "" FORCE)
  set(LLVM_USE_CRT_RELEASE "MD" CACHE STRING "" FORCE)
endif()
set(CMAKE_JOB_POOLS "objc3_link=1" CACHE STRING "" FORCE)
set(CMAKE_JOB_POOL_LINK "objc3_link" CACHE STRING "" FORCE)

# The standalone debugger has no dependency on a second Python ABI.
set(LLDB_ENABLE_PYTHON OFF CACHE BOOL "" FORCE)
set(LLDB_ENABLE_LUA OFF CACHE BOOL "" FORCE)

# Keep semantic editor services without compiling the optional tidy bundle.
set(CLANGD_TIDY_CHECKS OFF CACHE BOOL "" FORCE)

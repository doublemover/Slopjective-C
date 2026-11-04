cmake_minimum_required(VERSION 4.4.4)
project(objc3_implementation_analysis LANGUAGES CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_SYSTEM_NAME STREQUAL "Linux")
  message(FATAL_ERROR "Implementation analysis currently targets Linux")
endif()
set(OBJC3_ANALYSIS_FOUNDATION "" CACHE PATH "Admitted instrumented LLVM installation")
set(OBJC3_ANALYSIS_SOURCE "" CACHE PATH "Matching sealed LLVM source tree")
if(NOT OBJC3_ANALYSIS_FOUNDATION OR NOT OBJC3_ANALYSIS_SOURCE)
  message(FATAL_ERROR "Both matching foundation and source roots are required")
endif()
find_package(LLVM 23.1.3 EXACT REQUIRED CONFIG
  PATHS "${OBJC3_ANALYSIS_FOUNDATION}/lib/cmake/llvm" NO_DEFAULT_PATH)
find_package(Clang REQUIRED CONFIG
  PATHS "${OBJC3_ANALYSIS_FOUNDATION}/lib/cmake/clang" NO_DEFAULT_PATH)

# Compile the unchanged pinned engine/CLI and only the reviewed defect checkers.
# Link directly to the admitted fork DSOs; do not reconfigure the foundation or
# build every upstream module merely to expose three additional checks.
set(_tidy "${OBJC3_ANALYSIS_SOURCE}/clang-tools-extra/clang-tidy")
set(_sources
  ClangTidy.cpp ClangTidyCheck.cpp ClangTidyModule.cpp
  ClangTidyDiagnosticConsumer.cpp ClangTidyOptions.cpp ClangTidyProfiling.cpp
  ExpandModularHeadersPPCallbacks.cpp GlobList.cpp NoLintDirectiveHandler.cpp
  tool/ClangTidyMain.cpp tool/ClangTidyToolMain.cpp
  bugprone/UseAfterMoveCheck.cpp bugprone/DanglingHandleCheck.cpp
  bugprone/SizeofExpressionCheck.cpp
  utils/ASTUtils.cpp utils/ExprSequence.cpp utils/Matchers.cpp utils/OptionsUtils.cpp)
list(TRANSFORM _sources PREPEND "${_tidy}/")
set(CLANG_TIDY_ENABLE_STATIC_ANALYZER ON)
set(CLANG_TIDY_ENABLE_QUERY_BASED_CUSTOM_CHECKS OFF)
configure_file("${_tidy}/clang-tidy-config.h.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/clang-tidy-config.h")
add_executable(objc3-clang-tidy ${_sources} SelectedChecks.cpp)
# Upstream's force-link header names every optional module. This executable
# directly compiles its explicit registration TU, with no archive dead stripping.
set_source_files_properties("${_tidy}/tool/ClangTidyMain.cpp" PROPERTIES
  COMPILE_DEFINITIONS LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_CLANGTIDYFORCELINKER_H)
target_compile_features(objc3-clang-tidy PRIVATE cxx_std_20)
target_compile_options(objc3-clang-tidy PRIVATE
  -O1 -gline-tables-only -fno-omit-frame-pointer -fsanitize=address,undefined
  -fno-sanitize=function,vptr -fno-sanitize-recover=undefined)
target_link_options(objc3-clang-tidy PRIVATE
  -fuse-ld=lld -fsanitize=address,undefined -fno-sanitize-recover=undefined)
target_include_directories(objc3-clang-tidy PRIVATE
  "${CMAKE_CURRENT_BINARY_DIR}" "${_tidy}"
  ${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})
target_link_libraries(objc3-clang-tidy PRIVATE clang-cpp LLVM)
set_target_properties(objc3-clang-tidy PROPERTIES
  BUILD_RPATH "${OBJC3_ANALYSIS_FOUNDATION}/lib")

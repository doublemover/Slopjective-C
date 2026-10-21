cmake_minimum_required(VERSION 3.20)
cmake_policy(SET CMP0091 NEW)
project(objc3_native_runtime_cmake_fixture LANGUAGES CXX)

if (NOT OBJC3_NATIVE_RUNTIME_MODULE OR
    NOT OBJC3_TEST_RUNTIME_SOURCE OR
    NOT OBJC3_TEST_RUNTIME_BUILD OR
    NOT OBJC3_TEST_ROBIN_MAP_SOURCE)
  message(FATAL_ERROR "Pass the module, pinned runtime source/build, and robin-map source paths.")
endif()

include("${OBJC3_NATIVE_RUNTIME_MODULE}")
objc3c_configure_native_runtime(
  PROFILE windows-x86_64-libobjc2
  SOURCE_DIR "${OBJC3_TEST_RUNTIME_SOURCE}"
  BUILD_DIR "${OBJC3_TEST_RUNTIME_BUILD}"
  ROBIN_MAP_SOURCE_DIR "${OBJC3_TEST_ROBIN_MAP_SOURCE}")
objc3c_add_native_object_adapter(TARGET objc3_test_native_adapter)

get_filename_component(_repo_root "${CMAKE_CURRENT_LIST_DIR}/../../../.." ABSOLUTE)
add_executable(objc3_native_runtime_cmake_consumer native_runtime_cmake_consumer.cpp)
target_link_libraries(objc3_native_runtime_cmake_consumer PRIVATE
  objc3_test_native_adapter)
set_property(TARGET objc3_native_runtime_cmake_consumer PROPERTY
  MSVC_RUNTIME_LIBRARY MultiThreadedDLL)

objc3c_install_native_runtime(
  TARGET objc3_test_native_adapter
  DESTINATION h3-runtime)
install(TARGETS objc3_native_runtime_cmake_consumer
  RUNTIME DESTINATION h3-runtime/bin
  COMPONENT Runtime)

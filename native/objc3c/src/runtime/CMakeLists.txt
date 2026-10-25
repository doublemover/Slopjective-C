cmake_minimum_required(VERSION 4.4.4)

# Standalone runtime development does not need Clang libraries or a sealed SDK.
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  project(objc3_runtime LANGUAGES CXX)
  set(CMAKE_CXX_STANDARD 20)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(CMAKE_CXX_EXTENSIONS OFF)
  add_library(objc3c_build_defaults INTERFACE)
  target_include_directories(objc3c_build_defaults INTERFACE
    "${CMAKE_CURRENT_SOURCE_DIR}/..")
  include(CTest)
endif()

add_library(objc3_runtime_services STATIC
  services/error_service.cpp
  services/objc_task_error.cpp
  services/carrier_service.cpp
  concurrency/service_executor.cpp
  concurrency/service_task.cpp
  concurrency/service_group.cpp
  concurrency/service_actor.cpp
  concurrency/service_actor_object.cpp
  concurrency/service_public.cpp
  stdlib/text_service.cpp
  stdlib/keypath_service.cpp
  stdlib/collections_service.cpp
  stdlib/generic_collection_service.cpp
  stdlib/generic_associative_service.cpp)
target_link_libraries(objc3_runtime_services PUBLIC objc3c_build_defaults)
target_compile_options(objc3_runtime_services INTERFACE
  "$<$<COMPILE_LANG_AND_ID:CXX,Clang>:-fcoro-aligned-allocation>"
  "$<$<COMPILE_LANG_AND_ID:OBJCXX,Clang>:-fcoro-aligned-allocation>")
target_link_options(objc3_runtime_services INTERFACE
  "-fuse-ld=lld"
  "LINKER:-z,dead-reloc-in-nonalloc=.debug_addr=0xffffffffffffffff"
  "LINKER:-z,dead-reloc-in-nonalloc=.debug_info=0xffffffffffffffff")
target_include_directories(objc3_runtime_services PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/public>"
  "$<INSTALL_INTERFACE:include>")
find_package(Threads REQUIRED)
target_link_libraries(objc3_runtime_services PRIVATE Threads::Threads)


if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR AND BUILD_TESTING)
  get_filename_component(_repo "${CMAKE_CURRENT_SOURCE_DIR}/../../../.." ABSOLUTE)
  add_executable(objc3_task_services_test
    "${_repo}/tests/tooling/fixtures/clang_foundation/services/task_services.cpp"
    "${_repo}/tests/clang_foundation/structured_runtime/actor_scheduling.cpp"
    concurrency/service_group.cpp)
  target_compile_definitions(objc3_task_services_test PRIVATE OBJC3_RUNTIME_SERVICE_TESTING)
  add_executable(objc3_structured_runtime_test
    "${_repo}/tests/clang_foundation/structured_runtime/worker.cpp")
  add_executable(objc3_async_error_bridge_test
    "${_repo}/tests/clang_foundation/structured_runtime/error_bridge.cpp")
  add_executable(objc3_keypath_service_test ../../tests/keypath_service_test.cpp)
  foreach(_test task_services structured_runtime async_error_bridge keypath_service)
    target_link_libraries(objc3_${_test}_test PRIVATE objc3_runtime_services)
    target_compile_options(objc3_${_test}_test PRIVATE -UNDEBUG)
    add_test(NAME objc3_${_test} COMMAND objc3_${_test}_test)
    set_tests_properties(objc3_${_test} PROPERTIES TIMEOUT 30)
  endforeach()
endif()

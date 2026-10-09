if (MSVC)
  # Clang's generated option identifiers require standard macro expansion.
  target_compile_options(objc3c_build_defaults INTERFACE /Zc:preprocessor /Zc:__cplusplus)
endif()

add_library(objc3c_clang_foundation STATIC
  compile_session.cpp
  ast_contract_consumer.cpp
  interface_contract.cpp
  interface_codec.cpp
  interface_emitter.cpp
  interface_artifact_io.cpp
  interface_validation.cpp)
target_include_directories(objc3c_clang_foundation SYSTEM PRIVATE
  ${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})
target_link_libraries(objc3c_clang_foundation PUBLIC objc3c_build_defaults)
target_link_libraries(objc3c_clang_foundation PRIVATE
  clangTooling clangFrontend clangCodeGen clangIndex)
llvm_map_components_to_libnames(_objc3_native_codegen
  nativecodegen "${LLVM_NATIVE_ARCH}AsmParser")
target_link_libraries(objc3c_clang_foundation PRIVATE ${_objc3_native_codegen})
target_compile_definitions(objc3c_clang_foundation PRIVATE
  "OBJC3C_FOUNDATION_DRIVER=\"${OBJC3C_FOUNDATION_DRIVER}\""
  "OBJC3C_FOUNDATION_PATCH_DIGEST=\"${OBJC3C_FOUNDATION_PATCH_DIGEST}\""
  "OBJC3C_FOUNDATION_RESOURCE_DIR=\"${OBJC3C_FOUNDATION_RESOURCE_DIR}\"")

add_library(objc3c_frontend_v2 STATIC frontend_api.cpp)
target_link_libraries(objc3c_frontend_v2 PUBLIC objc3c_clang_foundation)
add_executable(objc3c-clang driver_main.cpp)
target_link_libraries(objc3c-clang PRIVATE objc3c_clang_foundation)

add_library(objc3_runtime_services STATIC
  ../runtime/services/error_service.cpp
  ../runtime/services/carrier_service.cpp
  ../runtime/concurrency/service_executor.cpp
  ../runtime/concurrency/service_task.cpp
  ../runtime/concurrency/service_group.cpp
  ../runtime/concurrency/service_actor.cpp
  ../runtime/stdlib/text_service.cpp
  ../runtime/stdlib/collections_service.cpp
  ../runtime/stdlib/generic_collection_service.cpp
  ../runtime/stdlib/generic_associative_service.cpp)
target_link_libraries(objc3_runtime_services PUBLIC objc3c_build_defaults)
target_include_directories(objc3_runtime_services PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}/../runtime/public")
find_package(Threads REQUIRED)
target_link_libraries(objc3_runtime_services PRIVATE Threads::Threads)

if (BUILD_TESTING)
  add_executable(objc3_carrier_service_test ../../tests/carrier_service_test.cpp)
  target_link_libraries(objc3_carrier_service_test PRIVATE objc3_runtime_services)
  add_test(NAME objc3_carrier_service COMMAND objc3_carrier_service_test)
  add_executable(objc3_interface_contract_test
    ../../../../tests/clang_foundation/interface_contract_test.cpp)
  target_link_libraries(objc3_interface_contract_test PRIVATE objc3c_clang_foundation)
  add_test(NAME objc3_interface_contract COMMAND objc3_interface_contract_test)
  add_executable(objc3_compile_session_test
    ../../../../tests/clang_foundation/compile_session_test.cpp)
  target_link_libraries(objc3_compile_session_test PRIVATE objc3c_frontend_v2)
  add_test(NAME objc3_compile_session COMMAND objc3_compile_session_test)
  add_executable(objc3_interface_emitter_test
    ../../../../tests/clang_foundation/interface_emitter_test.cpp)
  target_include_directories(objc3_interface_emitter_test SYSTEM PRIVATE
    ${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})
  target_link_libraries(objc3_interface_emitter_test PRIVATE objc3c_clang_foundation clangTooling)
  add_test(NAME objc3_interface_emitter COMMAND objc3_interface_emitter_test)
  add_executable(objc3_interface_artifact_io_test
    ../../../../tests/clang_foundation/interface_artifact_io_test.cpp)
  target_include_directories(objc3_interface_artifact_io_test SYSTEM PRIVATE
    ${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})
  target_link_libraries(objc3_interface_artifact_io_test PRIVATE objc3c_clang_foundation)
  add_test(NAME objc3_interface_artifact_io COMMAND objc3_interface_artifact_io_test)
  add_executable(objc3_task_services_test
    ../../../../tests/tooling/fixtures/clang_foundation/services/task_services.cpp
    ../runtime/concurrency/service_group.cpp)
  # Only this test translation unit enables deterministic fault hooks; the
  # production service library keeps them out of its ABI and implementation.
  target_compile_definitions(objc3_task_services_test PRIVATE OBJC3_RUNTIME_SERVICE_TESTING)
  target_link_libraries(objc3_task_services_test PRIVATE objc3_runtime_services)
  add_test(NAME objc3_task_services COMMAND objc3_task_services_test)
  set_tests_properties(objc3_task_services PROPERTIES TIMEOUT 30)
  add_executable(objc3_generic_collection_service_test
    ../../tests/generic_collection_service_test.cpp)
  target_link_libraries(objc3_generic_collection_service_test PRIVATE objc3_runtime_services)
  add_test(NAME objc3_generic_collection_service COMMAND objc3_generic_collection_service_test)
  add_executable(objc3_generic_associative_service_test
    ../../tests/generic_associative_service_test.cpp)
  target_link_libraries(objc3_generic_associative_service_test PRIVATE objc3_runtime_services)
  add_test(NAME objc3_generic_associative_service COMMAND objc3_generic_associative_service_test)
  foreach(_objc3_test objc3_carrier_service_test objc3_interface_contract_test objc3_compile_session_test
      objc3_interface_emitter_test objc3_interface_artifact_io_test objc3_task_services_test
      objc3_generic_collection_service_test objc3_generic_associative_service_test)
    if (MSVC)
      target_compile_options(${_objc3_test} PRIVATE /UNDEBUG)
    else()
      target_compile_options(${_objc3_test} PRIVATE -UNDEBUG)
    endif()
  endforeach()
endif()

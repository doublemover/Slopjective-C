# Shared real runtime regressions for standalone Debug/sanitizer builds and the
# complete native frontend inventory. Keep one owner for their registration.
function(objc3_add_generic_collection_tests)
  get_filename_component(_tests "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../tests" ABSOLUTE)
  add_executable(objc3_generic_collection_service_test
    ${_tests}/generic_collection_service_test.cpp)
  target_link_libraries(objc3_generic_collection_service_test PRIVATE objc3_runtime_services)
  add_test(NAME objc3_generic_collection_service COMMAND objc3_generic_collection_service_test)
  add_executable(objc3_generic_associative_service_test
    ${_tests}/generic_associative_service_test.cpp)
  target_link_libraries(objc3_generic_associative_service_test PRIVATE objc3_runtime_services)
  add_test(NAME objc3_generic_associative_service COMMAND objc3_generic_associative_service_test)
  add_executable(objc3_generic_collection_ownership_test
    ${_tests}/generic_collection_ownership_test.cpp
    ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../src/runtime/stdlib/generic_collection_service.cpp)
  target_compile_definitions(objc3_generic_collection_ownership_test PRIVATE OBJC3_GENERIC_ARRAY_STORAGE_TESTING)
  target_link_libraries(objc3_generic_collection_ownership_test PRIVATE objc3_runtime_services)
  target_link_options(objc3_generic_collection_ownership_test PRIVATE "LINKER:--wrap=_Znwm"
    "LINKER:--wrap=_ZnwmSt11align_val_tRKSt9nothrow_t")
  add_test(NAME objc3_generic_collection_ownership COMMAND objc3_generic_collection_ownership_test)
  set_tests_properties(objc3_generic_collection_ownership PROPERTIES TIMEOUT 30)
  foreach(_name collection_service associative_service collection_ownership)
    target_compile_options(objc3_generic_${_name}_test PRIVATE -UNDEBUG
      "SHELL:-include '${_tests}/test_assertion_policy.h'")
    set_tests_properties(objc3_generic_${_name} PROPERTIES TIMEOUT 30)
  endforeach()
endfunction()

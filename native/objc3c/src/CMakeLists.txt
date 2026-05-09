add_subdirectory(config)

# Dependency-direction authority is defined in `ARCHITECTURE.md` and checked
# by `scripts/check_objc3c_dependency_boundaries.py`. The hard-cutover graph uses
# concrete stage targets below while module extraction continues.

add_executable(objc3c-native
  main.cpp
)
objc3c_apply_build_defaults(objc3c-native)
objc3c_apply_llvm_direct_config(objc3c-native)
objc3c_apply_runtime_output(objc3c-native)

add_subdirectory(diag)
add_subdirectory(lex)

add_subdirectory(parse)
add_subdirectory(sema)

add_subdirectory(lower)

add_subdirectory(ir)

add_subdirectory(io)

add_subdirectory(pipeline)

add_subdirectory(artifacts)

add_subdirectory(runtime)

add_subdirectory(diagnostics)

add_subdirectory(libobjc3c_frontend)

add_subdirectory(driver)

target_link_libraries(objc3c-native PRIVATE
  objc3c_driver
)

add_subdirectory(tools)

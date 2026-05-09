add_library(objc3c_runtime_metadata INTERFACE)
target_link_libraries(objc3c_runtime_metadata INTERFACE
  objc3c_build_defaults
)

set(OBJC3_RUNTIME_LIBRARY_TARGET objc3_runtime)
set(OBJC3_RUNTIME_LIBRARY_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(OBJC3_RUNTIME_LIBRARY_PUBLIC_HEADER ${OBJC3_RUNTIME_LIBRARY_SOURCE_DIR}/public/objc3_runtime_api.h)
set(OBJC3_RUNTIME_LIBRARY_KIND STATIC)
set(OBJC3_RUNTIME_LIBRARY_ARCHIVE_BASENAME objc3_runtime)
# M251-D001 freeze: reserve the canonical in-tree native runtime library
# target/header/source-root surface before M251-D002 lands the actual library
# skeleton and M251-D003 wires the driver/link path to consume it.

add_library(${OBJC3_RUNTIME_LIBRARY_TARGET} ${OBJC3_RUNTIME_LIBRARY_KIND}
  blocks/block_capture_storage.cpp
  blocks/block_capture_storage.h
  blocks/block_record.cpp
  blocks/block_record.h
  classes/category_attachment.cpp
  classes/category_attachment.h
  classes/class_graph.cpp
  classes/class_graph.h
  classes/metaclass_graph.cpp
  classes/metaclass_graph.h
  classes/protocol_conformance.cpp
  classes/protocol_conformance.h
  concurrency/actor.cpp
  concurrency/actor.h
  concurrency/executor.cpp
  concurrency/executor.h
  concurrency/task.cpp
  concurrency/task.h
  dispatch/builtin_methods.cpp
  dispatch/builtin_methods.h
  dispatch/dispatch_api.cpp
  dispatch/dispatch_api.h
  dispatch/dispatch_errors.cpp
  dispatch/dispatch_errors.h
  dispatch/method_cache.cpp
  dispatch/method_cache.h
  dispatch/method_lookup.cpp
  dispatch/method_lookup.h
  errors/catch_filter.cpp
  errors/catch_filter.h
  errors/runtime_error.cpp
  errors/runtime_error.h
  images/image_descriptor.cpp
  images/image_descriptor.h
  images/multi_image_ordering.cpp
  images/multi_image_ordering.h
  images/registration.cpp
  images/registration.h
  memory/arc.cpp
  memory/arc.h
  memory/autorelease_pool.cpp
  memory/autorelease_pool.h
  objc3_runtime.cpp
  public/objc3_runtime_api.h
  public/objc3_runtime_result.h
  selectors/keypath_table.cpp
  selectors/keypath_table.h
  selectors/selector_table.cpp
  selectors/selector_table.h
  state/runtime_reset.cpp
  state/runtime_reset.h
  state/runtime_state.cpp
  state/runtime_state.h
  storage/instance_storage.cpp
  storage/instance_storage.h
  storage/ivar_layout.cpp
  storage/ivar_layout.h
  storage/property_accessors.cpp
  storage/property_accessors.h
  storage/weak_slots.cpp
  storage/weak_slots.h
  ../support/selectors/selector_normalization.cpp
  ../support/selectors/selector_normalization.h
)
objc3c_apply_build_defaults(${OBJC3_RUNTIME_LIBRARY_TARGET})
target_link_libraries(${OBJC3_RUNTIME_LIBRARY_TARGET} PUBLIC
  objc3c_runtime_metadata
)
set_target_properties(${OBJC3_RUNTIME_LIBRARY_TARGET} PROPERTIES
  OUTPUT_NAME ${OBJC3_RUNTIME_LIBRARY_ARCHIVE_BASENAME}
)
objc3c_apply_archive_output(${OBJC3_RUNTIME_LIBRARY_TARGET})
add_library(objc3c::runtime ALIAS ${OBJC3_RUNTIME_LIBRARY_TARGET})

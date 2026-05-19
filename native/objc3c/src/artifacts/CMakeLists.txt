include("${CMAKE_CURRENT_LIST_DIR}/cmake/artifact-json-schema-helpers.cmake")
target_sources(objc3c_artifacts_frontend_runtime PRIVATE
  objc3_frontend_runtime_import_type_contracts.h
)
include("${CMAKE_CURRENT_LIST_DIR}/cmake/artifact-core-sources.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/cmake/artifact-packet-manifest-support.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/cmake/artifact-target-properties.cmake")

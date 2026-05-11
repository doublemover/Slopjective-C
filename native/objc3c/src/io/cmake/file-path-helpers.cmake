set(OBJC3C_FILE_PATH_HELPER_SOURCES
  ../objc3_artifact_paths.cpp
  ../objc3_artifact_writers.cpp
  ../objc3_conformance_artifact_paths.cpp
  ../objc3_conformance_artifact_writers.cpp
  ../objc3_diagnostics_artifact_document.cpp
  ../objc3_diagnostics_artifacts.cpp
  ../objc3_file_io.cpp
  ../objc3_file_payload_io.cpp
  ../objc3_line_join.cpp
  ../objc3_runtime_artifact_paths.cpp
  ../objc3_runtime_artifact_writers.cpp
)
list(TRANSFORM OBJC3C_FILE_PATH_HELPER_SOURCES PREPEND "${CMAKE_CURRENT_LIST_DIR}/")

add_library(objc3c_io STATIC
  ${OBJC3C_FILE_PATH_HELPER_SOURCES}
)

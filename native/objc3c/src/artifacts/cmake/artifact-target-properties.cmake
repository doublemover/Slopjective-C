set(OBJC3C_ARTIFACT_BUILD_DEFAULT_TARGETS
  objc3c_artifacts_identity
  objc3c_artifacts_json
  objc3c_artifacts_evidence
  objc3c_artifacts_interop
  objc3c_artifacts_frontend_runtime
  objc3c_artifacts_frontend_conformance
  objc3c_artifacts_reports
  objc3c_artifacts
)

set(OBJC3C_ARTIFACT_LLVM_DIRECT_TARGETS
  objc3c_artifacts_json
  objc3c_artifacts_evidence
  objc3c_artifacts_interop
  objc3c_artifacts_frontend_runtime
  objc3c_artifacts_frontend_conformance
  objc3c_artifacts_reports
  objc3c_artifacts
)

foreach(target_name IN LISTS OBJC3C_ARTIFACT_BUILD_DEFAULT_TARGETS)
  objc3c_apply_build_defaults(${target_name})
endforeach()

foreach(target_name IN LISTS OBJC3C_ARTIFACT_LLVM_DIRECT_TARGETS)
  objc3c_apply_llvm_direct_config(${target_name})
endforeach()

target_link_libraries(objc3c_artifacts_json PUBLIC
  objc3c_ast
  objc3c_json
  objc3c_pipeline
  objc3c_support
)

target_link_libraries(objc3c_artifacts_evidence PUBLIC
  objc3c_json
  objc3c_pipeline
  objc3c_pipeline_results
)

target_link_libraries(objc3c_artifacts_interop PUBLIC
  objc3c_ast
  objc3c_json
  objc3c_pipeline
)

target_link_libraries(objc3c_artifacts_frontend_runtime PUBLIC
  objc3c_json
  objc3c_pipeline
  objc3c_support
)

target_link_libraries(objc3c_artifacts_frontend_conformance PUBLIC
  objc3c_artifacts_frontend_runtime
  objc3c_json
  objc3c_pipeline
  objc3c_support
)

target_link_libraries(objc3c_artifacts_reports PUBLIC
  objc3c_artifacts_frontend_conformance
  objc3c_artifacts_frontend_runtime
  objc3c_json
  objc3c_pipeline_results
)

target_link_libraries(objc3c_artifacts PUBLIC
  objc3c_artifacts_evidence
  objc3c_artifacts_frontend_conformance
  objc3c_artifacts_frontend_runtime
  objc3c_artifacts_identity
  objc3c_artifacts_interop
  objc3c_artifacts_json
  objc3c_artifacts_reports
  objc3c_pipeline
  objc3c_support
)

unset(OBJC3C_ARTIFACT_BUILD_DEFAULT_TARGETS)
unset(OBJC3C_ARTIFACT_LLVM_DIRECT_TARGETS)

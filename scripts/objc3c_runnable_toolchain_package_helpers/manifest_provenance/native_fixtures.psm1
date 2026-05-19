Set-StrictMode -Version Latest

function Get-ManifestProvenanceNativeSmokeFixtureFiles {
  return @(
    "tests/tooling/fixtures/native/hello.objc3",
    "tests/tooling/fixtures/native/negative_undefined_symbol.objc3"
  )
}

function Get-ManifestProvenanceNativeRuntimeFixtureFiles {
  return @(
    "tests/tooling/fixtures/native/canonical_runnable_sample_set.objc3",
    "tests/tooling/fixtures/native/runtime_canonical_runnable_object_runtime_library.objc3",
    "tests/tooling/fixtures/native/live_dispatch_fast_path_positive.objc3",
    "tests/tooling/fixtures/native/arc_property_interaction_positive.objc3",
    "tests/tooling/fixtures/native/synthesized_accessor_property_lowering_positive.objc3",
    "tests/tooling/fixtures/native/runtime_metadata_source_records_class_protocol_property_ivar.objc3",
    "tests/tooling/fixtures/native/byref_cell_copy_dispose_runtime_positive.objc3",
    "tests/tooling/fixtures/native/runtime_backed_storage_ownership_reflection_positive.objc3",
    "tests/tooling/fixtures/native/live_error_runtime_integration_positive.objc3",
    "tests/tooling/fixtures/native/live_continuation_runtime_integration_positive.objc3",
    "tests/tooling/fixtures/native/live_task_runtime_and_executor_implementation_positive.objc3",
    "tests/tooling/fixtures/native/actor_lowering_runtime_positive.objc3",
    "tests/tooling/fixtures/native/macro_host_process_provider.objc3",
    "tests/tooling/fixtures/native/macro_host_process_consumer.objc3",
    "tests/tooling/fixtures/native/bridge_packaging_toolchain_provider.objc3",
    "tests/tooling/fixtures/native/bridge_packaging_toolchain_consumer.objc3",
    "tests/tooling/fixtures/native/header_module_bridge_provider.objc3",
    "tests/tooling/fixtures/native/header_module_bridge_consumer.objc3"
  )
}

Export-ModuleMember -Function @(
  "Get-ManifestProvenanceNativeSmokeFixtureFiles",
  "Get-ManifestProvenanceNativeRuntimeFixtureFiles"
)

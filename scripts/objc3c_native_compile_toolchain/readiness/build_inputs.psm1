$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function Get-NativeCompilerBuildInputPaths {
  param([Parameter(Mandatory = $true)][string]$RepoRoot)

  return @(
    (Join-Path $RepoRoot "native/objc3c/src/main.cpp")
    (Join-Path $RepoRoot "native/objc3c/src/contracts/objc3_frontend_diagnostics_bus_contract.h")
    (Join-Path $RepoRoot "native/objc3c/src/diag/objc3_diag_utils.h")
    (Join-Path $RepoRoot "native/objc3c/src/diag/objc3_diag_utils.cpp")
    (Join-Path $RepoRoot "native/objc3c/src/support/objc3_ascii_predicates.h")
    (Join-Path $RepoRoot "native/objc3c/src/support/objc3_file_reading.h")
    (Join-Path $RepoRoot "native/objc3c/src/support/objc3_identifier_safe_suffix.h")
    (Join-Path $RepoRoot "native/objc3c/src/support/objc3_ir_object_backend_token.h")
    (Join-Path $RepoRoot "native/objc3c/src/support/objc3_method_family.h")
    (Join-Path $RepoRoot "native/objc3c/src/support/objc3_runtime_dispatch_symbol.h")
    (Join-Path $RepoRoot "native/objc3c/src/support/objc3_runtime_metadata_record_set.h")
    (Join-Path $RepoRoot "native/objc3c/src/support/objc3_string_predicates.h")
    (Join-Path $RepoRoot "native/objc3c/src/support/objc3_value_type_names.h")
    (Join-Path $RepoRoot "native/objc3c/src/pipeline/objc3_frontend_types.h")
    (Join-Path $RepoRoot "native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp")
    (Join-Path $RepoRoot "native/objc3c/src/io/objc3_json.h")
    (Join-Path $RepoRoot "native/objc3c/src/io/objc3_json.cpp")
    (Join-Path $RepoRoot "native/objc3c/src/io/objc3_process.h")
    (Join-Path $RepoRoot "native/objc3c/src/io/objc3_process.cpp")
    (Join-Path $RepoRoot "native/objc3c/src/driver/objc3_objc3_path.cpp")
    (Join-Path $RepoRoot "native/objc3c/src/libobjc3c_frontend/frontend_anchor.cpp")
    (Join-Path $RepoRoot "native/objc3c/src/runtime/objc3_runtime.cpp")
    (Join-Path $RepoRoot "native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h")
    (Join-Path $RepoRoot "scripts/build_objc3c_native.ps1")
  )
}

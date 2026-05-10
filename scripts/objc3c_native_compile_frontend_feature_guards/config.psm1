$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function Get-FrontendCoreFeatureGuardConfig {
  [pscustomobject]@{
    artifact_name = "frontend core feature expansion"
    contract_id = "objc3c-frontend-build-invocation-core-feature-expansion/parser_build-core-feature-expansion-v1"
    dependency_contract_ids = @(
      "objc3c-frontend-build-invocation-modular-scaffold/parser_build-modular-scaffold-v1",
      "objc3c-frontend-build-invocation-manifest-guard/parser_build-manifest-guard-v1"
    )
    required_modules = @("driver", "diagnostics-io", "ir", "lex-parse", "frontend-api", "lowering", "pipeline", "sema")
    default_out_dir = "tmp/artifacts/compilation/objc3c-native"
    cache_root = "tmp/artifacts/objc3c-native/cache"
    capability_summary_flag = "--llvm-capabilities-summary"
    route_flag = "--objc3-route-backend-from-capabilities"
    allowed_ir_object_backends = @("clang", "llvm-direct")
  }
}

function Get-FrontendEdgeCompatibilityGuardConfig {
  [pscustomobject]@{
    artifact_name = "frontend edge compatibility"
    contract_id = "objc3c-frontend-build-invocation-edge-compat-completion/parser_build-edge-compat-completion-v1"
    dependency_contract_ids = @(
      "objc3c-frontend-build-invocation-core-feature-expansion/parser_build-core-feature-expansion-v1",
      "objc3c-frontend-build-invocation-manifest-guard/parser_build-manifest-guard-v1"
    )
    fail_closed_exit_code = 2
    disallow_relative_parent_segments = $true
    capability_summary_flag = "--llvm-capabilities-summary"
    route_flag = "--objc3-route-backend-from-capabilities"
    required_single_value_flags = @("--objc3-ir-object-backend", "--llvm-capabilities-summary")
    backend_flag = "--objc3-ir-object-backend"
    allowed_boolean_true_values = @("1", "true", "yes", "on")
    allowed_boolean_false_values = @("0", "false", "no", "off")
  }
}

function Get-FrontendFeatureGuardBooleanConfig {
  [pscustomobject]@{
    true_values = @("1", "true", "yes", "on")
    false_values = @("0", "false", "no", "off")
  }
}

Export-ModuleMember -Function @(
  "Get-FrontendCoreFeatureGuardConfig",
  "Get-FrontendEdgeCompatibilityGuardConfig",
  "Get-FrontendFeatureGuardBooleanConfig"
)

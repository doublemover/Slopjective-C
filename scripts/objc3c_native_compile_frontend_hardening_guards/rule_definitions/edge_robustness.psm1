$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function Get-FrontendEdgeRobustnessGuardRules {
  [pscustomobject]@{
    artifact_name = "frontend edge robustness"
    contract_id = "objc3c-frontend-build-invocation-edge-robustness/parser_build-edge-robustness-v1"
    dependency_contract_ids = @(
      "objc3c-frontend-build-invocation-edge-compat-completion/parser_build-edge-compat-completion-v1",
      "objc3c-frontend-build-invocation-core-feature-expansion/parser_build-core-feature-expansion-v1"
    )
    wrapper_single_value_flags = @("--use-cache", "--out-dir")
    compile_single_value_flags = @(
      "--objc3-ir-object-backend",
      "--llvm-capabilities-summary",
      "--objc3-route-backend-from-capabilities"
    )
    reject_empty_equals_value_flags = @("--emit-prefix", "--clang", "--use-cache")
    report_key = "edge_robustness_path"
  }
}

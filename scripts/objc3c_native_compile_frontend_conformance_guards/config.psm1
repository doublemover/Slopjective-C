$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function Get-FrontendConformanceRejectDiagnostics {
  @(
    "--objc3-route-backend-from-capabilities requires --llvm-capabilities-summary",
    "unsupported value '<backend>' for --objc3-ir-object-backend",
    "--objc3-ir-object-backend can be provided at most once",
    "--llvm-capabilities-summary must not contain '..' relative segments",
    "--objc3-route-backend-from-capabilities can be provided at most once"
  )
}

function Get-FrontendConformanceInvocationConfig {
  [pscustomobject]@{
    backend_flag = "--objc3-ir-object-backend"
    capability_summary_flag = "--llvm-capabilities-summary"
    route_flag = "--objc3-route-backend-from-capabilities"
    allowed_ir_object_backends = @("clang", "llvm-direct")
    allowed_boolean_true_values = @("1", "true", "yes", "on")
    allowed_boolean_false_values = @("0", "false", "no", "off")
    cache_modes = @("no-cache", "cache-aware")
    backend_modes = @("default", "clang", "llvm-direct")
    summary_modes = @("none", "present")
  }
}

function Get-FrontendConformanceMatrixGuardConfig {
  [pscustomobject]@{
    artifact_name = "frontend conformance matrix"
    contract_id = "objc3c-frontend-build-invocation-conformance-matrix/parser_build-conformance-matrix-v1"
    dependency_contract_ids = @(
      "objc3c-frontend-build-invocation-recovery-determinism-hardening/parser_build-recovery-determinism-hardening-v1",
      "objc3c-frontend-build-invocation-edge-compat-completion/parser_build-edge-compat-completion-v1"
    )
    required_rejection_diagnostics = @(Get-FrontendConformanceRejectDiagnostics)
  }
}

function Get-FrontendConformanceCorpusGuardConfig {
  [pscustomobject]@{
    artifact_name = "frontend conformance corpus"
    contract_id = "objc3c-frontend-build-invocation-conformance-corpus/parser_build-conformance-corpus-v1"
    dependency_contract_ids = @(
      "objc3c-frontend-build-invocation-conformance-matrix/parser_build-conformance-matrix-v1",
      "objc3c-frontend-build-invocation-edge-compat-completion/parser_build-edge-compat-completion-v1"
    )
    required_rejection_diagnostics = @(Get-FrontendConformanceRejectDiagnostics)
  }
}

function Get-FrontendIntegrationCloseoutGuardConfig {
  [pscustomobject]@{
    artifact_name = "frontend integration closeout"
    contract_id = "objc3c-frontend-build-invocation-integration-closeout/parser_build-integration-closeout-v1"
    dependency_contract_ids = @(
      "objc3c-frontend-build-invocation-conformance-corpus/parser_build-conformance-corpus-v1",
      "objc3c-frontend-build-invocation-conformance-matrix/parser_build-conformance-matrix-v1",
      "objc3c-frontend-build-invocation-recovery-determinism-hardening/parser_build-recovery-determinism-hardening-v1"
    )
    deterministic_fail_closed_exit_code = 2
  }
}

Export-ModuleMember -Function @(
  "Get-FrontendConformanceCorpusGuardConfig",
  "Get-FrontendConformanceInvocationConfig",
  "Get-FrontendConformanceMatrixGuardConfig",
  "Get-FrontendConformanceRejectDiagnostics",
  "Get-FrontendIntegrationCloseoutGuardConfig"
)

$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

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

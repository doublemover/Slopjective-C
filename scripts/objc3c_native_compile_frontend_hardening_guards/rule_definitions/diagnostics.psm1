$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function Get-FrontendDiagnosticsHardeningGuardRules {
  [pscustomobject]@{
    artifact_name = "frontend diagnostics hardening"
    contract_id = "objc3c-frontend-build-invocation-diagnostics-hardening/parser_build-diagnostics-hardening-v1"
    dependency_contract_ids = @(
      "objc3c-frontend-build-invocation-edge-robustness/parser_build-edge-robustness-v1",
      "objc3c-frontend-build-invocation-edge-compat-completion/parser_build-edge-compat-completion-v1"
    )
    fail_closed_exit_code = 2
    required_error_messages = @(
      "--use-cache can be provided at most once",
      "invalid --use-cache value",
      "--out-dir can be provided at most once",
      "missing value for --out-dir",
      "empty value for --out-dir",
      "missing value for --emit-prefix",
      "empty value for --emit-prefix",
      "missing value for --clang",
      "empty value for --clang"
    )
    report_key = "diagnostics_hardening_path"
  }
}

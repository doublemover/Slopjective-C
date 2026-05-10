Set-StrictMode -Version Latest

. (Join-Path $PSScriptRoot "paths_config.psm1")
. (Join-Path $PSScriptRoot "file_io.psm1")
. (Join-Path $PSScriptRoot "contract_assertions.psm1")
. (Join-Path $PSScriptRoot "source_scanning.psm1")
. (Join-Path $PSScriptRoot "runtime_orchestration.psm1")
. (Join-Path $PSScriptRoot "reporting.psm1")
. (Join-Path $PSScriptRoot "runner.psm1")

Export-ModuleMember -Function @(
  "Invoke-SemaPassManagerDiagnosticsBusContract"
)

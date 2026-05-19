$sourceScanningRoot = Join-Path $PSScriptRoot "source_scanning"

. (Join-Path $sourceScanningRoot "file_assertions.psm1")
. (Join-Path $sourceScanningRoot "snapshot.psm1")
. (Join-Path $sourceScanningRoot "static_header_contracts.psm1")
. (Join-Path $sourceScanningRoot "static_source_contracts.psm1")
. (Join-Path $sourceScanningRoot "static_pipeline_contracts.psm1")
. (Join-Path $sourceScanningRoot "static_contracts.psm1")

if ($MyInvocation.InvocationName -ne ".") {
  Export-ModuleMember -Function @(
    "Assert-SemaPassManagerDiagnosticsBusSourceFiles",
    "Read-SemaPassManagerDiagnosticsBusSourceSnapshot",
    "Assert-SemaPassManagerDiagnosticsBusStaticContracts"
  )
}

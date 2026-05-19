function Assert-SemaPassManagerDiagnosticsBusStaticContracts {
  param([Parameter(Mandatory = $true)][object]$Snapshot)

  Assert-SemaPassManagerDiagnosticsBusHeaderContracts -Snapshot $Snapshot
  Assert-SemaPassManagerDiagnosticsBusSourceContracts -Snapshot $Snapshot
  Assert-SemaPassManagerDiagnosticsBusPipelineContracts -Snapshot $Snapshot
}

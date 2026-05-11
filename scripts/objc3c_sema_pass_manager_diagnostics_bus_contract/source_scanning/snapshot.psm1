function Read-SemaPassManagerDiagnosticsBusSourceSnapshot {
  param([Parameter(Mandatory = $true)][object]$Config)

  $sources = $Config.sources
  return [pscustomobject]@{
    sema_header = Read-NormalizedText -Path $sources.sema_header
    sema_contract_header = Read-NormalizedText -Path $sources.sema_contract_header
    sema_pass_manager_contract_header = Read-NormalizedText -Path $sources.sema_pass_manager_contract_header
    sema_pass_manager_header = Read-NormalizedText -Path $sources.sema_pass_manager_header
    sema_pass_manager_source = Read-NormalizedText -Path $sources.sema_pass_manager_source
    sema_diagnostics_bus_header = Read-NormalizedText -Path $sources.sema_diagnostics_bus_header
    parse_diagnostics_bus_header = Read-NormalizedText -Path $sources.parse_diagnostics_bus_header
    sema_source = Read-NormalizedText -Path $sources.sema_source
    sema_pure_contract_source = Read-NormalizedText -Path $sources.sema_pure_contract_source
    pipeline_source = Read-NormalizedText -Path $sources.pipeline_source
    frontend_types = Read-NormalizedText -Path $sources.frontend_types
    cmake = Read-NormalizedText -Path $sources.cmake
  }
}

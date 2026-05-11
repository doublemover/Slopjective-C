function Assert-SemaPassManagerDiagnosticsBusSourceFiles {
  param([Parameter(Mandatory = $true)][object]$Config)

  $sources = $Config.sources
  Assert-FileExists -Path $sources.sema_header -Id "source.sema_header.exists" -Description "sema header"
  Assert-FileExists -Path $sources.sema_contract_header -Id "source.sema_contract_header.exists" -Description "sema contract header"
  Assert-FileExists -Path $sources.sema_pass_manager_contract_header -Id "source.sema_pass_manager_contract_header.exists" -Description "sema pass-manager contract header"
  Assert-FileExists -Path $sources.sema_pass_manager_header -Id "source.sema_pass_manager_header.exists" -Description "sema pass-manager header"
  Assert-FileExists -Path $sources.sema_pass_manager_source -Id "source.sema_pass_manager_source.exists" -Description "sema pass-manager source"
  Assert-FileExists -Path $sources.sema_diagnostics_bus_header -Id "source.sema_diagnostics_bus_header.exists" -Description "sema diagnostics-bus header"
  Assert-FileExists -Path $sources.sema_diagnostics_bus_source -Id "source.sema_diagnostics_bus_source.exists" -Description "sema diagnostics-bus source"
  Assert-FileExists -Path $sources.sema_source -Id "source.sema_source.exists" -Description "sema source"
  Assert-FileExists -Path $sources.sema_pure_contract_source -Id "source.sema_pure_contract_source.exists" -Description "sema pure-contract source"
  Assert-FileExists -Path $sources.sema_static_analysis_header -Id "source.sema_static_analysis_header.exists" -Description "sema static analysis header"
  Assert-FileExists -Path $sources.sema_static_analysis_source -Id "source.sema_static_analysis_source.exists" -Description "sema static analysis source"
  Assert-FileExists -Path $sources.parse_diagnostics_bus_header -Id "source.parse_diagnostics_bus_header.exists" -Description "parse diagnostics-bus header"
  Assert-FileExists -Path $sources.pipeline_source -Id "source.pipeline_source.exists" -Description "pipeline source"
  Assert-FileExists -Path $sources.frontend_types -Id "source.frontend_types.exists" -Description "frontend types"
  Assert-FileExists -Path $sources.cmake -Id "source.cmake.exists" -Description "native CMake file"
  Assert-FileExists -Path $Config.fixtures.positive -Id "fixture.positive.exists" -Description "positive sema fixture"
  foreach ($negativeFixturePath in $Config.fixtures.negative) {
    Assert-FileExists `
      -Path $negativeFixturePath `
      -Id ("fixture.negative.exists.{0}" -f [System.IO.Path]::GetFileNameWithoutExtension($negativeFixturePath)) `
      -Description "negative sema fixture"
  }
}

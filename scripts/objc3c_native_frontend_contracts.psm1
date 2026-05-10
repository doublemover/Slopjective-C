$ErrorActionPreference = "Stop"

function Get-Objc3cNativeFrontendArtifactPaths {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot
  )

  return [pscustomobject]@{
    SourceGraph = Join-Path $RepoRoot "tmp/artifacts/objc3c-native/frontend_source_graph.json"
    InvocationLock = Join-Path $RepoRoot "tmp/artifacts/objc3c-native/frontend_invocation_lock.json"
    CoreFeatureExpansion = Join-Path $RepoRoot "tmp/artifacts/objc3c-native/frontend_core_feature_expansion.json"
    EdgeCompatibility = Join-Path $RepoRoot "tmp/artifacts/objc3c-native/frontend_edge_compat.json"
    EdgeRobustness = Join-Path $RepoRoot "tmp/artifacts/objc3c-native/frontend_edge_robustness.json"
    DiagnosticsHardening = Join-Path $RepoRoot "tmp/artifacts/objc3c-native/frontend_diagnostics_hardening.json"
    RecoveryDeterminismHardening = Join-Path $RepoRoot "tmp/artifacts/objc3c-native/frontend_recovery_determinism_hardening.json"
    ConformanceMatrix = Join-Path $RepoRoot "tmp/artifacts/objc3c-native/frontend_conformance_matrix.json"
    ConformanceCorpus = Join-Path $RepoRoot "tmp/artifacts/objc3c-native/frontend_conformance_corpus.json"
    IntegrationCloseout = Join-Path $RepoRoot "tmp/artifacts/objc3c-native/frontend_integration_closeout.json"
  }
}

function Get-Objc3cNativeFrontendPacketDefinitions {
  param(
    [Parameter(Mandatory = $true)]$ArtifactPaths
  )

  return @(
    [pscustomobject]@{
      Name = "frontend_source_graph"
      Family = "source-derived"
      OutputPath = $ArtifactPaths.SourceGraph
      Dependencies = @()
      RequiresNativeBinaries = $false
    }
    [pscustomobject]@{
      Name = "frontend_invocation_lock"
      Family = "binary-derived"
      OutputPath = $ArtifactPaths.InvocationLock
      Dependencies = @("frontend_source_graph")
      RequiresNativeBinaries = $true
    }
    [pscustomobject]@{
      Name = "frontend_core_feature_expansion"
      Family = "binary-derived"
      OutputPath = $ArtifactPaths.CoreFeatureExpansion
      Dependencies = @("frontend_source_graph", "frontend_invocation_lock")
      RequiresNativeBinaries = $true
    }
    [pscustomobject]@{
      Name = "frontend_edge_compat"
      Family = "closeout-derived"
      OutputPath = $ArtifactPaths.EdgeCompatibility
      Dependencies = @("frontend_core_feature_expansion")
      RequiresNativeBinaries = $false
    }
    [pscustomobject]@{
      Name = "frontend_edge_robustness"
      Family = "closeout-derived"
      OutputPath = $ArtifactPaths.EdgeRobustness
      Dependencies = @("frontend_edge_compat")
      RequiresNativeBinaries = $false
    }
    [pscustomobject]@{
      Name = "frontend_diagnostics_hardening"
      Family = "closeout-derived"
      OutputPath = $ArtifactPaths.DiagnosticsHardening
      Dependencies = @("frontend_edge_robustness")
      RequiresNativeBinaries = $false
    }
    [pscustomobject]@{
      Name = "frontend_recovery_determinism_hardening"
      Family = "closeout-derived"
      OutputPath = $ArtifactPaths.RecoveryDeterminismHardening
      Dependencies = @("frontend_diagnostics_hardening")
      RequiresNativeBinaries = $false
    }
    [pscustomobject]@{
      Name = "frontend_conformance_matrix"
      Family = "closeout-derived"
      OutputPath = $ArtifactPaths.ConformanceMatrix
      Dependencies = @("frontend_recovery_determinism_hardening")
      RequiresNativeBinaries = $false
    }
    [pscustomobject]@{
      Name = "frontend_conformance_corpus"
      Family = "closeout-derived"
      OutputPath = $ArtifactPaths.ConformanceCorpus
      Dependencies = @("frontend_conformance_matrix")
      RequiresNativeBinaries = $false
    }
    [pscustomobject]@{
      Name = "frontend_integration_closeout"
      Family = "closeout-derived"
      OutputPath = $ArtifactPaths.IntegrationCloseout
      Dependencies = @("frontend_conformance_corpus")
      RequiresNativeBinaries = $false
    }
  )
}

function Get-Objc3cNativeSelectedFrontendPacketDefinitions {
  param(
    [Parameter(Mandatory = $true)][string]$Mode,
    [Parameter(Mandatory = $true)][object[]]$PacketDefinitions
  )

  switch ($Mode) {
    "contracts-source" {
      return @($PacketDefinitions | Where-Object { $_.Family -eq "source-derived" })
    }
    "contracts-binary" {
      return @($PacketDefinitions | Where-Object { $_.Family -in @("source-derived", "binary-derived") })
    }
    "contracts-closeout" {
      return @($PacketDefinitions | Where-Object { $_.Family -in @("source-derived", "binary-derived", "closeout-derived") })
    }
    "contracts-all" {
      return @($PacketDefinitions)
    }
    "full" {
      return @($PacketDefinitions)
    }
    default {
      return @()
    }
  }
}

function Assert-Objc3cNativeFrontendPacketPrerequisites {
  param(
    [Parameter(Mandatory = $true)][object[]]$PacketDefinitions,
    [Parameter(Mandatory = $true)][object[]]$SelectedPacketDefinitions,
    [Parameter(Mandatory = $true)][string]$NativeBinaryPath,
    [Parameter(Mandatory = $true)][string]$CapiBinaryPath
  )

  $selectedNames = @{}
  foreach ($definition in $SelectedPacketDefinitions) {
    $selectedNames[$definition.Name] = $true
  }

  foreach ($definition in $SelectedPacketDefinitions) {
    if ($definition.RequiresNativeBinaries) {
      foreach ($binaryPath in @($NativeBinaryPath, $CapiBinaryPath)) {
        if (!(Test-Path -LiteralPath $binaryPath -PathType Leaf)) {
          throw ("contract artifact family '{0}' requires existing native binaries: {1}" -f $definition.Family, $binaryPath)
        }
      }
    }
    foreach ($dependencyName in $definition.Dependencies) {
      if ($selectedNames.ContainsKey($dependencyName)) {
        continue
      }
      $dependency = $PacketDefinitions | Where-Object { $_.Name -eq $dependencyName } | Select-Object -First 1
      if ($null -eq $dependency) {
        throw ("frontend contract dependency not declared: " + $dependencyName)
      }
      if (!(Test-Path -LiteralPath $dependency.OutputPath -PathType Leaf)) {
        throw ("packet '{0}' requires existing dependency output '{1}' at {2}" -f $definition.Name, $dependencyName, $dependency.OutputPath)
      }
    }
  }
}

function Get-Objc3cNativeFrontendModules {
  return @(
    [ordered]@{
      name = "driver"
      sources = @(
        "native/objc3c/src/driver/objc3_cli_options.cpp"
        "native/objc3c/src/driver/objc3_driver_main.cpp"
        "native/objc3c/src/driver/objc3_driver_shell.cpp"
        "native/objc3c/src/driver/objc3_frontend_options.cpp"
        "native/objc3c/src/driver/objc3_llvm_capability_routing.cpp"
        "native/objc3c/src/driver/objc3_objc3_path.cpp"
        "native/objc3c/src/driver/objc3_objectivec_path.cpp"
        "native/objc3c/src/driver/objc3_compilation_driver.cpp"
      )
    }
    [ordered]@{
      name = "diagnostics-io"
      sources = @(
        "native/objc3c/src/diag/objc3_diag_utils.cpp"
        "native/objc3c/src/io/objc3_diagnostics_artifacts.cpp"
        "native/objc3c/src/io/objc3_file_io.cpp"
        "native/objc3c/src/io/objc3_json.cpp"
        "native/objc3c/src/io/objc3_manifest_artifacts.cpp"
        "native/objc3c/src/io/objc3_process.cpp"
      )
    }
    [ordered]@{
      name = "ir"
      sources = @(
        "native/objc3c/src/ir/objc3_ir_emitter.cpp"
      )
    }
    [ordered]@{
      name = "lex-parse"
      sources = @(
        "native/objc3c/src/lex/objc3_lexer.cpp"
        "native/objc3c/src/parse/objc3_ast_builder.cpp"
        "native/objc3c/src/parse/objc3_ast_builder_contract.cpp"
        "native/objc3c/src/parse/objc3_diagnostic_grammar_hooks_core_feature.cpp"
        "native/objc3c/src/parse/objc3_diagnostic_source_precision_scaffold.cpp"
        "native/objc3c/src/parse/objc3_parse_support.cpp"
        "native/objc3c/src/parse/objc3_parser.cpp"
      )
    }
    [ordered]@{
      name = "frontend-api"
      sources = @(
        "native/objc3c/src/libobjc3c_frontend/c_api.cpp"
        "native/objc3c/src/libobjc3c_frontend/frontend_anchor.cpp"
        "native/objc3c/src/libobjc3c_frontend/objc3_cli_frontend.cpp"
      )
    }
    [ordered]@{
      name = "lowering"
      sources = @(
        "native/objc3c/src/lower/objc3_lowering_contract.cpp"
      )
    }
    [ordered]@{
      name = "pipeline"
      sources = @(
        "native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp"
        "native/objc3c/src/pipeline/objc3_runtime_import_surface.cpp"
        "native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp"
        "native/objc3c/src/pipeline/objc3_ir_emission_completeness_scaffold.cpp"
        "native/objc3c/src/pipeline/objc3_lowering_pipeline_pass_graph_core_feature_surface.cpp"
        "native/objc3c/src/pipeline/objc3_lowering_pipeline_pass_graph_scaffold.cpp"
      )
    }
    [ordered]@{
      name = "sema"
      sources = @(
        "native/objc3c/src/sema/objc3_sema_diagnostics_bus.cpp"
        "native/objc3c/src/sema/objc3_sema_pass_flow_scaffold.cpp"
        "native/objc3c/src/sema/objc3_sema_pass_manager.cpp"
        "native/objc3c/src/sema/objc3_semantic_passes.cpp"
        "native/objc3c/src/sema/objc3_type_form_scaffold.cpp"
        "native/objc3c/src/sema/objc3_static_analysis.cpp"
        "native/objc3c/src/sema/objc3_pure_contract.cpp"
      )
    }
  )
}

function Get-Objc3cNativeFrontendSharedSources {
  param([object[]]$Modules)

  $seen = @{}
  $flattened = New-Object System.Collections.Generic.List[string]
  foreach ($module in $Modules) {
    $name = [string]$module.name
    $sources = @($module.sources)
    if ([string]::IsNullOrWhiteSpace($name)) {
      throw "frontend module entry missing name"
    }
    if ($sources.Count -eq 0) {
      throw "frontend module '$name' must declare at least one source"
    }
    foreach ($source in $sources) {
      if ($seen.ContainsKey($source)) {
        throw "duplicate frontend shared source entry: $source"
      }
      $seen[$source] = $true
      $flattened.Add($source)
    }
  }
  return $flattened.ToArray()
}

Export-ModuleMember -Function @(
  "Get-Objc3cNativeFrontendArtifactPaths",
  "Get-Objc3cNativeFrontendPacketDefinitions",
  "Get-Objc3cNativeSelectedFrontendPacketDefinitions",
  "Assert-Objc3cNativeFrontendPacketPrerequisites",
  "Get-Objc3cNativeFrontendModules",
  "Get-Objc3cNativeFrontendSharedSources"
)

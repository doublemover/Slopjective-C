$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function ConvertTo-NativeCompilerBuildResult {
  param(
    [object[]]$BuildOutput,
    [int]$ExitCode
  )

  $buildOutputLines = New-Object System.Collections.Generic.List[string]
  $artifactRelativePaths = [ordered]@{
    frontend_source_graph_relative_path = $null
    frontend_invocation_lock_relative_path = $null
    frontend_core_feature_expansion_relative_path = $null
    frontend_edge_compat_relative_path = $null
    frontend_edge_robustness_relative_path = $null
    frontend_diagnostics_hardening_relative_path = $null
    frontend_recovery_determinism_hardening_relative_path = $null
    frontend_conformance_matrix_relative_path = $null
    frontend_conformance_corpus_relative_path = $null
    frontend_integration_closeout_relative_path = $null
  }
  $outputPrefixes = [ordered]@{
    "frontend_source_graph=" = "frontend_source_graph_relative_path"
    "frontend_invocation_lock=" = "frontend_invocation_lock_relative_path"
    "frontend_core_feature_expansion=" = "frontend_core_feature_expansion_relative_path"
    "frontend_edge_compat=" = "frontend_edge_compat_relative_path"
    "frontend_edge_robustness=" = "frontend_edge_robustness_relative_path"
    "frontend_diagnostics_hardening=" = "frontend_diagnostics_hardening_relative_path"
    "frontend_recovery_determinism_hardening=" = "frontend_recovery_determinism_hardening_relative_path"
    "frontend_conformance_matrix=" = "frontend_conformance_matrix_relative_path"
    "frontend_conformance_corpus=" = "frontend_conformance_corpus_relative_path"
    "frontend_integration_closeout=" = "frontend_integration_closeout_relative_path"
  }

  foreach ($line in @($BuildOutput)) {
    $lineText = [string]$line
    $buildOutputLines.Add($lineText)
    foreach ($prefix in $outputPrefixes.Keys) {
      if ($lineText.StartsWith($prefix)) {
        $artifactRelativePaths[$outputPrefixes[$prefix]] = $lineText.Substring($prefix.Length).Trim()
      }
    }
  }

  return [pscustomobject]@{
    exit_code = [int]$ExitCode
    build_output_lines = $buildOutputLines.ToArray()
    frontend_source_graph_relative_path = $artifactRelativePaths["frontend_source_graph_relative_path"]
    frontend_invocation_lock_relative_path = $artifactRelativePaths["frontend_invocation_lock_relative_path"]
    frontend_core_feature_expansion_relative_path = $artifactRelativePaths["frontend_core_feature_expansion_relative_path"]
    frontend_edge_compat_relative_path = $artifactRelativePaths["frontend_edge_compat_relative_path"]
    frontend_edge_robustness_relative_path = $artifactRelativePaths["frontend_edge_robustness_relative_path"]
    frontend_diagnostics_hardening_relative_path = $artifactRelativePaths["frontend_diagnostics_hardening_relative_path"]
    frontend_recovery_determinism_hardening_relative_path = $artifactRelativePaths["frontend_recovery_determinism_hardening_relative_path"]
    frontend_conformance_matrix_relative_path = $artifactRelativePaths["frontend_conformance_matrix_relative_path"]
    frontend_conformance_corpus_relative_path = $artifactRelativePaths["frontend_conformance_corpus_relative_path"]
    frontend_integration_closeout_relative_path = $artifactRelativePaths["frontend_integration_closeout_relative_path"]
  }
}

Export-ModuleMember -Function "ConvertTo-NativeCompilerBuildResult"

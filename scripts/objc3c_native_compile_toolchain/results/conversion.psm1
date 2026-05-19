$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function ConvertTo-NativeCompilerBuildResult {
  param(
    [object[]]$BuildOutput,
    [int]$ExitCode
  )

  $buildOutputLines = New-Object System.Collections.Generic.List[string]
  $artifactRelativePaths = New-NativeCompilerBuildArtifactRelativePathMap
  $outputPrefixes = Get-NativeCompilerBuildOutputPrefixes

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

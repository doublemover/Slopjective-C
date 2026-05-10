function Resolve-ValidatedRunId {
  param(
    [Parameter()][string]$ConfiguredRunId,
    [Parameter(Mandatory = $true)][string]$DefaultRunId
  )

  if ([string]::IsNullOrWhiteSpace($ConfiguredRunId)) {
    return $DefaultRunId
  }

  $candidate = $ConfiguredRunId.Trim()
  if ($candidate.Length -gt 80) {
    throw "sema extraction FAIL: configured run id exceeds 80 characters"
  }
  if ($candidate -notmatch '^[A-Za-z0-9_-]+$') {
    throw "sema extraction FAIL: configured run id must match ^[A-Za-z0-9_-]+$"
  }
  return $candidate
}

function Get-RepoRelativePath {
  param(
    [Parameter(Mandatory = $true)][string]$Path,
    [Parameter(Mandatory = $true)][string]$Root
  )

  $fullPath = [System.IO.Path]::GetFullPath("$Path")
  $fullRoot = [System.IO.Path]::GetFullPath("$Root")
  if ($fullPath.StartsWith($fullRoot, [System.StringComparison]::OrdinalIgnoreCase)) {
    return $fullPath.Substring($fullRoot.Length).TrimStart('\', '/').Replace('\', '/')
  }
  return $fullPath.Replace('\', '/')
}

function New-SemaPassManagerDiagnosticsBusContractConfig {
  param(
    [Parameter(Mandatory = $true)][string]$ScriptRoot,
    [Parameter()][string]$ConfiguredRunId,
    [Parameter()][string]$ConfiguredNativeExe
  )

  $repoRoot = (Resolve-Path (Join-Path $ScriptRoot "..")).Path
  $suiteRoot = Join-Path $repoRoot "tmp/artifacts/compilation/objc3c-native/typed_abi/sema-pass-manager-diagnostics-bus-contract"
  $defaultRunId = "typed_abi-sema-type-system-default"
  $runId = Resolve-ValidatedRunId -ConfiguredRunId $ConfiguredRunId -DefaultRunId $defaultRunId
  $runDir = Join-Path $suiteRoot $runId
  $runDirRel = "tmp/artifacts/compilation/objc3c-native/typed_abi/sema-pass-manager-diagnostics-bus-contract/$runId"
  $summaryRel = "$runDirRel/summary.json"

  $defaultNativeExePath = Join-Path $repoRoot "artifacts/bin/objc3c-native.exe"
  $nativeExePath = if ([string]::IsNullOrWhiteSpace($ConfiguredNativeExe)) { $defaultNativeExePath } else { $ConfiguredNativeExe }
  $nativeExeExplicit = -not [string]::IsNullOrWhiteSpace($ConfiguredNativeExe)

  return [pscustomobject]@{
    repo_root = $repoRoot
    run_id = $runId
    run_dir = $runDir
    run_dir_rel = $runDirRel
    summary_path = Join-Path $runDir "summary.json"
    summary_rel = $summaryRel
    build_script_path = Join-Path $repoRoot "scripts/build_objc3c_native.ps1"
    native_exe_path = $nativeExePath
    native_exe_explicit = $nativeExeExplicit
    sources = [pscustomobject]@{
      sema_header = Join-Path $repoRoot "native/objc3c/src/sema/objc3_semantic_passes.h"
      sema_contract_header = Join-Path $repoRoot "native/objc3c/src/sema/objc3_sema_contract.h"
      sema_pass_manager_contract_header = Join-Path $repoRoot "native/objc3c/src/sema/objc3_sema_pass_manager_contract.h"
      sema_pass_manager_header = Join-Path $repoRoot "native/objc3c/src/sema/objc3_sema_pass_manager.h"
      sema_pass_manager_source = Join-Path $repoRoot "native/objc3c/src/sema/objc3_sema_pass_manager.cpp"
      sema_diagnostics_bus_header = Join-Path $repoRoot "native/objc3c/src/sema/objc3_sema_diagnostics_bus.h"
      sema_diagnostics_bus_source = Join-Path $repoRoot "native/objc3c/src/sema/objc3_sema_diagnostics_bus.cpp"
      sema_source = Join-Path $repoRoot "native/objc3c/src/sema/objc3_semantic_passes.cpp"
      sema_pure_contract_source = Join-Path $repoRoot "native/objc3c/src/sema/objc3_pure_contract.cpp"
      sema_static_analysis_header = Join-Path $repoRoot "native/objc3c/src/sema/objc3_static_analysis.h"
      sema_static_analysis_source = Join-Path $repoRoot "native/objc3c/src/sema/objc3_static_analysis.cpp"
      parse_diagnostics_bus_header = Join-Path $repoRoot "native/objc3c/src/parse/objc3_diagnostics_bus.h"
      pipeline_source = Join-Path $repoRoot "native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp"
      frontend_types = Join-Path $repoRoot "native/objc3c/src/pipeline/objc3_frontend_types.h"
      cmake = Join-Path $repoRoot "native/objc3c/CMakeLists.txt"
    }
    fixtures = [pscustomobject]@{
      positive = Join-Path $repoRoot "tests/tooling/fixtures/native/recovery/positive/typed_i32_bool.objc3"
      negative = @(
        (Join-Path $repoRoot "tests/tooling/fixtures/native/recovery/negative/negative_type_mismatch.objc3"),
        (Join-Path $repoRoot "tests/tooling/fixtures/native/recovery/negative/negative_pure_definition_impure_message_send.objc3")
      )
    }
  }
}

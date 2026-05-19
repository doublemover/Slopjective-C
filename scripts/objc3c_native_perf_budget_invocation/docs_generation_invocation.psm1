Set-StrictMode -Version Latest

$objc3cNativePerfBudgetScriptRoot = Split-Path -Parent $PSScriptRoot
Import-Module (Join-Path $objc3cNativePerfBudgetScriptRoot "objc3c_native_perf_budget_helpers.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "process_invocation.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "report_rendering.psm1") -Force -DisableNameChecking

function Invoke-Objc3cNativePerfDocsGeneratorProof {
  param(
    [object]$Config,
    [ref]$DocsGenerationProof
  )

  if (!(Test-Path -LiteralPath $Config.native_docs_script -PathType Leaf)) {
    throw "perf-budget FAIL: missing native docs generator at $($Config.native_docs_script)"
  }
  if (!(Test-Path -LiteralPath $Config.command_surface_script -PathType Leaf)) {
    throw "perf-budget FAIL: missing public command surface generator at $($Config.command_surface_script)"
  }
  $docsDir = Join-Path $Config.run_dir "docs-generation"
  New-Item -ItemType Directory -Force -Path $docsDir | Out-Null
  $nativeDocsLog = Join-Path $docsDir "native-docs.log"
  $commandSurfaceLog = Join-Path $docsDir "public-command-surface.log"
  $nativeDocsRun = Invoke-Objc3cNativePerfNativeProcess `
    -Command $Config.python_command `
    -Arguments @($Config.native_docs_script) `
    -LogPath $nativeDocsLog
  if ($nativeDocsRun.exit_code -ne 0) {
    throw "perf-budget FAIL: native docs generation failed with exit code $($nativeDocsRun.exit_code)"
  }
  $commandSurfaceRun = Invoke-Objc3cNativePerfNativeProcess `
    -Command $Config.python_command `
    -Arguments @($Config.command_surface_script) `
    -LogPath $commandSurfaceLog
  if ($commandSurfaceRun.exit_code -ne 0) {
    throw "perf-budget FAIL: public command surface generation failed with exit code $($commandSurfaceRun.exit_code)"
  }

  $proof = [ordered]@{
    executed = $true
    status = "PASS"
    detail = "checked-in docs generators executed on the live repo surface"
    native_docs = [ordered]@{
      elapsed_ms = $nativeDocsRun.elapsed_ms
      exit_code = $nativeDocsRun.exit_code
      log = (Get-RepoRelativePath -Path $nativeDocsLog -Root $Config.repo_root)
    }
    public_command_surface = [ordered]@{
      elapsed_ms = $commandSurfaceRun.elapsed_ms
      exit_code = $commandSurfaceRun.exit_code
      log = (Get-RepoRelativePath -Path $commandSurfaceLog -Root $Config.repo_root)
    }
  }
  Write-Objc3cNativePerfDocsGenerationLine -NativeDocsElapsedMs $nativeDocsRun.elapsed_ms -CommandSurfaceElapsedMs $commandSurfaceRun.elapsed_ms

  $DocsGenerationProof.Value = $proof
}

Export-ModuleMember -Function @(
  "Invoke-Objc3cNativePerfDocsGeneratorProof"
)

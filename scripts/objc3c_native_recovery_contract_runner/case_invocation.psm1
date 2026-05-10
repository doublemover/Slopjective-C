$ErrorActionPreference = "Stop"

$script:ScriptsRoot = Split-Path -Parent $PSScriptRoot
Import-Module (Join-Path $script:ScriptsRoot "objc3c_native_recovery_contract_helpers.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "context.psm1") -Force -DisableNameChecking

function Invoke-ContractCase {
  param(
    [string]$Source,
    [string]$CaseName,
    [switch]$RequireLl,
    [switch]$RequireCompileProvenance,
    [switch]$UseCompileWrapper,
    [string[]]$ExtraArgs = @(),
    [string[]]$RequiredLlTokens = @("define i32 @objc3c_entry"),
    [string[]]$ForbiddenLlTokens = @(),
    [string[]]$RequiredManifestTokens = @(),
    [switch]$RequireObjc3ManifestSurface
  )

  $context = Get-RecoveryContractContext
  $repoRoot = $context.RepoRoot
  $outDir = $context.OutDir

  $resolvedSource = if ([System.IO.Path]::IsPathRooted($Source)) { $Source } else { Join-Path $repoRoot $Source }
  if (-not (Test-Path -LiteralPath $resolvedSource -PathType Leaf)) {
    throw "contract FAIL: missing source fixture for $CaseName at $resolvedSource"
  }

  $run1 = Join-Path $outDir ($CaseName + "_run1")
  $run2 = Join-Path $outDir ($CaseName + "_run2")
  New-Item -ItemType Directory -Force -Path $run1 | Out-Null
  New-Item -ItemType Directory -Force -Path $run2 | Out-Null

  $compileArgsRun1 = @($resolvedSource, "--out-dir", $run1, "--emit-prefix", "module") + $ExtraArgs
  $compileArgsRun2 = @($resolvedSource, "--out-dir", $run2, "--emit-prefix", "module") + $ExtraArgs

  $exitRun1 = Invoke-Objc3cNativeWithRecovery -Arguments $compileArgsRun1 -UseCompileWrapper:$UseCompileWrapper
  if ($exitRun1 -ne 0) { throw "contract FAIL: compile failed for $CaseName run1" }
  $exitRun2 = Invoke-Objc3cNativeWithRecovery -Arguments $compileArgsRun2 -UseCompileWrapper:$UseCompileWrapper
  if ($exitRun2 -ne 0) { throw "contract FAIL: compile failed for $CaseName run2" }

  $manifest1 = Get-Content -LiteralPath (Join-Path $run1 "module.manifest.json") -Raw
  $manifest2 = Get-Content -LiteralPath (Join-Path $run2 "module.manifest.json") -Raw
  $diag1 = Get-Content -LiteralPath (Join-Path $run1 "module.diagnostics.txt") -Raw
  $diag2 = Get-Content -LiteralPath (Join-Path $run2 "module.diagnostics.txt") -Raw

  if ($manifest1 -ne $manifest2) { throw "contract FAIL: manifest drift across replay for $CaseName" }
  if ($diag1 -ne $diag2) { throw "contract FAIL: diagnostics drift across replay for $CaseName" }
  if ($RequireCompileProvenance) {
    $provenance1 = Assert-CompileOutputProvenance -CaseName "$CaseName run1" -RunDir $run1
    $provenance2 = Assert-CompileOutputProvenance -CaseName "$CaseName run2" -RunDir $run2
    if ($provenance1 -ne $provenance2) { throw "contract FAIL: compile provenance drift across replay for $CaseName" }
  }
  foreach ($token in $RequiredManifestTokens) {
    if ([string]::IsNullOrWhiteSpace($token)) {
      continue
    }
    if ($manifest1.IndexOf($token, [System.StringComparison]::Ordinal) -lt 0) {
      throw "contract FAIL: missing manifest token '$token' in run1 for $CaseName"
    }
    if ($manifest2.IndexOf($token, [System.StringComparison]::Ordinal) -lt 0) {
      throw "contract FAIL: missing manifest token '$token' in run2 for $CaseName"
    }
  }
  if ($RequireObjc3ManifestSurface) {
    Assert-Objc3ManifestPipelineSurface -ManifestText $manifest1 -CaseName "$CaseName run1"
    Assert-Objc3ManifestPipelineSurface -ManifestText $manifest2 -CaseName "$CaseName run2"
  }

  $objPath = Join-Path $run1 "module.obj"
  if (!(Test-Path -LiteralPath $objPath -PathType Leaf)) { throw "contract FAIL: missing object artifact for $CaseName" }
  $objSize = (Get-Item -LiteralPath $objPath).Length
  if ($objSize -le 0) { throw "contract FAIL: empty object artifact for $CaseName" }

  if ($RequireLl) {
    $ll1 = Get-Content -LiteralPath (Join-Path $run1 "module.ll") -Raw
    $ll2 = Get-Content -LiteralPath (Join-Path $run2 "module.ll") -Raw
    $ll1Code = (($ll1 -split "`r?`n") | Where-Object { $_ -notmatch '^\s*;' }) -join "`n"
    $ll2Code = (($ll2 -split "`r?`n") | Where-Object { $_ -notmatch '^\s*;' }) -join "`n"
    $ll1Entrypoints = Get-EntrypointLlSurface -LlText $ll1Code
    $ll2Entrypoints = Get-EntrypointLlSurface -LlText $ll2Code
    if ($ll1 -ne $ll2) { throw "contract FAIL: LLVM IR drift across replay for $CaseName" }
    foreach ($token in $RequiredLlTokens) {
      if ([string]::IsNullOrWhiteSpace($token)) {
        continue
      }
      if ($ll1Code.IndexOf($token, [System.StringComparison]::Ordinal) -lt 0) {
        throw "contract FAIL: missing LLVM IR token '$token' in run1 for $CaseName"
      }
      if ($ll2Code.IndexOf($token, [System.StringComparison]::Ordinal) -lt 0) {
        throw "contract FAIL: missing LLVM IR token '$token' in run2 for $CaseName"
      }
    }
    foreach ($token in $ForbiddenLlTokens) {
      if ([string]::IsNullOrWhiteSpace($token)) {
        continue
      }
      if ($ll1Entrypoints.IndexOf($token, [System.StringComparison]::Ordinal) -ge 0) {
        throw "contract FAIL: forbidden LLVM IR token '$token' present in run1 for $CaseName"
      }
      if ($ll2Entrypoints.IndexOf($token, [System.StringComparison]::Ordinal) -ge 0) {
        throw "contract FAIL: forbidden LLVM IR token '$token' present in run2 for $CaseName"
      }
    }
    Write-Output "$CaseName`_deterministic_ir=true"
  }

  Write-Output "$CaseName`_deterministic_manifest=true"
  if ($RequireCompileProvenance) {
    Write-Output "$CaseName`_deterministic_compile_provenance=true"
  }
  Write-Output "$CaseName`_deterministic_diagnostics=true"
  Write-Output "$CaseName`_object_size=$objSize"
}

Export-ModuleMember -Function @(
  "Invoke-ContractCase"
)

$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest
if ($PSVersionTable.PSVersion.Major -ge 7) {
  $PSNativeCommandUseErrorActionPreference = $false
}

Import-Module (Join-Path $PSScriptRoot "../objc3c_driver_shell_split_contract_helpers.psm1") -DisableNameChecking

function Assert-Objc3cDriverShellSplitRegex {
  param(
    [Parameter(Mandatory = $true)][string]$Text,
    [Parameter(Mandatory = $true)][string]$Pattern,
    [Parameter(Mandatory = $true)][string]$Id,
    [Parameter(Mandatory = $true)][string]$FailureMessage,
    [Parameter(Mandatory = $true)][string]$PassMessage
  )

  Assert-Contract `
    -Condition ([regex]::IsMatch($Text, $Pattern)) `
    -Id $Id `
    -FailureMessage $FailureMessage `
    -PassMessage $PassMessage
}

function Assert-Objc3cDriverShellSplitNoRegex {
  param(
    [Parameter(Mandatory = $true)][string]$Text,
    [Parameter(Mandatory = $true)][string]$Pattern,
    [Parameter(Mandatory = $true)][string]$Id,
    [Parameter(Mandatory = $true)][string]$FailureMessage,
    [Parameter(Mandatory = $true)][string]$PassMessage
  )

  Assert-Contract `
    -Condition (-not [regex]::IsMatch($Text, $Pattern)) `
    -Id $Id `
    -FailureMessage $FailureMessage `
    -PassMessage $PassMessage
}

function Assert-Objc3cDriverShellSplitTextContains {
  param(
    [Parameter(Mandatory = $true)][string]$Text,
    [Parameter(Mandatory = $true)][string]$Token,
    [Parameter(Mandatory = $true)][string]$Id,
    [Parameter(Mandatory = $true)][string]$FailureMessage,
    [Parameter(Mandatory = $true)][string]$PassMessage
  )

  Assert-Contract `
    -Condition ($Text.IndexOf($Token, [System.StringComparison]::Ordinal) -ge 0) `
    -Id $Id `
    -FailureMessage $FailureMessage `
    -PassMessage $PassMessage
}

function Assert-Objc3cDriverShellSplitExitCode {
  param(
    [Parameter(Mandatory = $true)][int]$ActualExitCode,
    [Parameter(Mandatory = $true)][int]$ExpectedExitCode,
    [Parameter(Mandatory = $true)][string]$Id,
    [Parameter(Mandatory = $true)][string]$FailureMessage,
    [Parameter(Mandatory = $true)][string]$PassMessage,
    [Parameter()][object]$Evidence = $null
  )

  Assert-Contract `
    -Condition ($ActualExitCode -eq $ExpectedExitCode) `
    -Id $Id `
    -FailureMessage $FailureMessage `
    -PassMessage $PassMessage `
    -Evidence $Evidence
}

function Assert-Objc3cDriverShellSplitNativeExecutableReady {
  param([Parameter(Mandatory = $true)]$Config)

  if (-not $Config.NativeExeExplicit -and !(Test-Path -LiteralPath $Config.NativeExePath -PathType Leaf)) {
    $buildLog = Join-Path $Config.RunDir "build.log"
    $buildExit = Invoke-LoggedCommand `
      -Command "powershell" `
      -Arguments @("-NoProfile", "-ExecutionPolicy", "Bypass", "-File", $Config.BuildScriptPath) `
      -LogPath $buildLog
    Assert-Contract `
      -Condition ($buildExit -eq 0) `
      -Id "smoke.build.native_executable" `
      -FailureMessage ("native build failed with exit={0} (log={1})" -f $buildExit, (Get-RepoRelativePath -Path $buildLog -Root $Config.RepoRoot)) `
      -PassMessage "native executable build completed" `
      -Evidence @{ exit_code = $buildExit; log = (Get-RepoRelativePath -Path $buildLog -Root $Config.RepoRoot) }
  }

  Assert-FileExists -Path $Config.NativeExePath -Id "smoke.native_executable.exists" -Description "native executable"
}

function Assert-Objc3cDriverShellSplitArtifactPair {
  param(
    [Parameter(Mandatory = $true)]$Config,
    [Parameter(Mandatory = $true)][string]$ArtifactName,
    [Parameter(Mandatory = $true)][string]$Run1ArtifactPath,
    [Parameter(Mandatory = $true)][string]$Run2ArtifactPath
  )

  $run1Exists = Test-Path -LiteralPath $Run1ArtifactPath -PathType Leaf
  $run2Exists = Test-Path -LiteralPath $Run2ArtifactPath -PathType Leaf
  Assert-Contract `
    -Condition ($run1Exists -and $run2Exists) `
    -Id ("smoke.artifact.exists.{0}" -f $ArtifactName) `
    -FailureMessage ("expected artifact missing across replay for {0}" -f $ArtifactName) `
    -PassMessage ("artifact present across replay: {0}" -f $ArtifactName)

  if ($ArtifactName -eq "module.obj") {
    $run1Size = (Get-Item -LiteralPath $Run1ArtifactPath).Length
    $run2Size = (Get-Item -LiteralPath $Run2ArtifactPath).Length
    Assert-Contract `
      -Condition ($run1Size -gt 0 -and $run2Size -gt 0) `
      -Id "smoke.artifact.nonempty.module.obj" `
      -FailureMessage "module.obj is empty in one or more compile runs" `
      -PassMessage "module.obj is non-empty across replay" `
      -Evidence @{ run1_bytes = $run1Size; run2_bytes = $run2Size }
  }
}

function Assert-Objc3cDriverShellSplitArtifactDigest {
  param(
    [Parameter(Mandatory = $true)][string]$ArtifactName,
    [Parameter(Mandatory = $true)][string]$Run1Hash,
    [Parameter(Mandatory = $true)][string]$Run2Hash
  )

  if ($ArtifactName -eq "module.obj") {
    Add-Check `
      -Id "smoke.artifact.hash_recorded.module.obj" `
      -Passed $true `
      -Detail "module.obj hashes captured for diagnostics; determinism is not enforced in this gate" `
      -Evidence @{ run1_sha256 = $Run1Hash; run2_sha256 = $Run2Hash }
    return
  }

  Assert-Contract `
    -Condition ($Run1Hash -eq $Run2Hash) `
    -Id ("smoke.artifact.deterministic_sha256.{0}" -f $ArtifactName) `
    -FailureMessage ("artifact hash drift across replay for {0}" -f $ArtifactName) `
    -PassMessage ("artifact hash stable across replay: {0}" -f $ArtifactName) `
    -Evidence @{ run1_sha256 = $Run1Hash; run2_sha256 = $Run2Hash }
}

Export-ModuleMember -Function @(
  "Assert-Objc3cDriverShellSplitArtifactDigest",
  "Assert-Objc3cDriverShellSplitArtifactPair",
  "Assert-Objc3cDriverShellSplitExitCode",
  "Assert-Objc3cDriverShellSplitNativeExecutableReady",
  "Assert-Objc3cDriverShellSplitNoRegex",
  "Assert-Objc3cDriverShellSplitRegex",
  "Assert-Objc3cDriverShellSplitTextContains"
)

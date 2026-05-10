$ErrorActionPreference = "Stop"

$script:ScriptsRoot = Split-Path -Parent $PSScriptRoot
Import-Module (Join-Path $script:ScriptsRoot "objc3c_native_recovery_contract_helpers.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "path_config_helpers.psm1") -Force -DisableNameChecking

function Assert-RecoveryContractTextReplay {
  param(
    [AllowEmptyString()][string]$Run1Text,
    [AllowEmptyString()][string]$Run2Text,
    [string]$FailureMessage
  )

  if ($Run1Text -ne $Run2Text) {
    throw $FailureMessage
  }
}

function Assert-RecoveryContractCompileProvenanceReplay {
  param(
    [string]$CaseName,
    [string]$Run1,
    [string]$Run2
  )

  $provenance1 = Assert-CompileOutputProvenance -CaseName "$CaseName run1" -RunDir $Run1
  $provenance2 = Assert-CompileOutputProvenance -CaseName "$CaseName run2" -RunDir $Run2
  if ($provenance1 -ne $provenance2) {
    throw "contract FAIL: compile provenance drift across replay for $CaseName"
  }
}

function Assert-RecoveryContractManifestTokens {
  param(
    [string]$CaseName,
    [AllowEmptyString()][string]$Manifest1,
    [AllowEmptyString()][string]$Manifest2,
    [string[]]$RequiredManifestTokens = @()
  )

  foreach ($token in $RequiredManifestTokens) {
    if ([string]::IsNullOrWhiteSpace($token)) {
      continue
    }
    if ($Manifest1.IndexOf($token, [System.StringComparison]::Ordinal) -lt 0) {
      throw "contract FAIL: missing manifest token '$token' in run1 for $CaseName"
    }
    if ($Manifest2.IndexOf($token, [System.StringComparison]::Ordinal) -lt 0) {
      throw "contract FAIL: missing manifest token '$token' in run2 for $CaseName"
    }
  }
}

function Assert-RecoveryContractManifestSurface {
  param(
    [string]$CaseName,
    [AllowEmptyString()][string]$Manifest1,
    [AllowEmptyString()][string]$Manifest2
  )

  Assert-Objc3ManifestPipelineSurface -ManifestText $Manifest1 -CaseName "$CaseName run1"
  Assert-Objc3ManifestPipelineSurface -ManifestText $Manifest2 -CaseName "$CaseName run2"
}

function Assert-RecoveryContractObjectArtifact {
  param(
    [string]$CaseName,
    [string]$RunDir
  )

  $objPath = Get-RecoveryContractArtifactPath -RunDir $RunDir -FileName "module.obj"
  if (!(Test-Path -LiteralPath $objPath -PathType Leaf)) { throw "contract FAIL: missing object artifact for $CaseName" }
  $objSize = (Get-Item -LiteralPath $objPath).Length
  if ($objSize -le 0) { throw "contract FAIL: empty object artifact for $CaseName" }

  return $objSize
}

function Get-RecoveryContractLlCode {
  param(
    [AllowEmptyString()][string]$LlText
  )

  return (($LlText -split "`r?`n") | Where-Object { $_ -notmatch '^\s*;' }) -join "`n"
}

function Assert-RecoveryContractLlExpectations {
  param(
    [string]$CaseName,
    [string]$Run1,
    [string]$Run2,
    [string[]]$RequiredLlTokens = @("define i32 @objc3c_entry"),
    [string[]]$ForbiddenLlTokens = @()
  )

  $ll1 = Get-Content -LiteralPath (Get-RecoveryContractArtifactPath -RunDir $Run1 -FileName "module.ll") -Raw
  $ll2 = Get-Content -LiteralPath (Get-RecoveryContractArtifactPath -RunDir $Run2 -FileName "module.ll") -Raw
  $ll1Code = Get-RecoveryContractLlCode -LlText $ll1
  $ll2Code = Get-RecoveryContractLlCode -LlText $ll2
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
}

Export-ModuleMember -Function @(
  "Assert-RecoveryContractCompileProvenanceReplay",
  "Assert-RecoveryContractLlExpectations",
  "Assert-RecoveryContractManifestSurface",
  "Assert-RecoveryContractManifestTokens",
  "Assert-RecoveryContractObjectArtifact",
  "Assert-RecoveryContractTextReplay"
)

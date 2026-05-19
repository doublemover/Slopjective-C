function Get-SemaPassManagerNegativeRuntimeForbiddenArtifacts {
  return @(
    "module.manifest.json",
    "module.ll",
    "module.obj",
    "module.object-backend.txt"
  )
}

function Get-SemaPassManagerNegativeRuntimeUnavailableMarkers {
  return @(
    "llvm-direct object emission failed: llc executable not found:",
    "llvm-direct object emission backend unavailable in this build",
    "llvm-direct object emission failed: llc exited with status "
  )
}

function Test-SemaPassManagerNegativeRuntimeUnavailableLog {
  param(
    [Parameter(Mandatory = $true)]
    [string]$LogText
  )

  foreach ($marker in Get-SemaPassManagerNegativeRuntimeUnavailableMarkers) {
    if ($LogText.IndexOf($marker, [System.StringComparison]::Ordinal) -ge 0) {
      return $true
    }
  }

  return $false
}

function Get-SemaPassManagerNegativeRuntimeDiagnosticsPaths {
  param(
    [Parameter(Mandatory = $true)]
    [string]$OutputDir
  )

  return [pscustomobject]@{
    Txt = Join-Path $OutputDir "module.diagnostics.txt"
    Json = Join-Path $OutputDir "module.diagnostics.json"
  }
}

function Assert-SemaPassManagerNegativeRuntimeDiagnosticsExist {
  param(
    [Parameter(Mandatory = $true)]
    [string]$FixtureStem,
    [Parameter(Mandatory = $true)]
    [string]$ContractId,
    [Parameter(Mandatory = $true)]
    [string]$FailureMessage,
    [Parameter(Mandatory = $true)]
    [string]$PassMessage,
    [Parameter(Mandatory = $true)]
    $LeftDiagnostics,
    [Parameter(Mandatory = $true)]
    $RightDiagnostics
  )

  Assert-Contract `
    -Condition ((Test-Path -LiteralPath $LeftDiagnostics.Txt -PathType Leaf) -and
                (Test-Path -LiteralPath $RightDiagnostics.Txt -PathType Leaf) -and
                (Test-Path -LiteralPath $LeftDiagnostics.Json -PathType Leaf) -and
                (Test-Path -LiteralPath $RightDiagnostics.Json -PathType Leaf)) `
    -Id ($ContractId -f $FixtureStem) `
    -FailureMessage ($FailureMessage -f $FixtureStem) `
    -PassMessage ($PassMessage -f $FixtureStem)
}

function Assert-SemaPassManagerNegativeRuntimeExpectedCodesPresent {
  param(
    [Parameter(Mandatory = $true)]
    [string]$FixtureStem,
    [Parameter(Mandatory = $true)]
    [string[]]$ExpectedCodes,
    [Parameter(Mandatory = $true)]
    [string]$ContractId,
    [Parameter(Mandatory = $true)]
    [string]$FailureMessage,
    [Parameter(Mandatory = $true)]
    [string]$PassMessage,
    [Parameter(Mandatory = $true)]
    [string]$LeftTxtText,
    [Parameter(Mandatory = $true)]
    [string]$RightTxtText,
    [Parameter(Mandatory = $true)]
    [string]$LeftJsonText,
    [Parameter(Mandatory = $true)]
    [string]$RightJsonText
  )

  foreach ($expectedCode in $ExpectedCodes) {
    Assert-Contract `
      -Condition ($LeftTxtText.IndexOf($expectedCode, [System.StringComparison]::Ordinal) -ge 0 -and
                  $RightTxtText.IndexOf($expectedCode, [System.StringComparison]::Ordinal) -ge 0 -and
                  $LeftJsonText.IndexOf($expectedCode, [System.StringComparison]::Ordinal) -ge 0 -and
                  $RightJsonText.IndexOf($expectedCode, [System.StringComparison]::Ordinal) -ge 0) `
      -Id ($ContractId -f $FixtureStem, $expectedCode) `
      -FailureMessage ($FailureMessage -f $expectedCode, $FixtureStem) `
      -PassMessage ($PassMessage -f $expectedCode, $FixtureStem)
  }
}

function Assert-SemaPassManagerNegativeRuntimeForbiddenArtifactsAbsent {
  param(
    [Parameter(Mandatory = $true)]
    [string]$FixtureStem,
    [Parameter(Mandatory = $true)]
    [string]$ContractId,
    [Parameter(Mandatory = $true)]
    [string]$FailureMessage,
    [Parameter(Mandatory = $true)]
    [string]$PassMessage,
    [Parameter(Mandatory = $true)]
    [string[]]$OutputDirs
  )

  foreach ($forbiddenArtifact in Get-SemaPassManagerNegativeRuntimeForbiddenArtifacts) {
    $forbiddenPresent = $false
    foreach ($outputDir in $OutputDirs) {
      $artifactPath = Join-Path $outputDir $forbiddenArtifact
      if (Test-Path -LiteralPath $artifactPath -PathType Leaf) {
        $forbiddenPresent = $true
        break
      }
    }

    Assert-Contract `
      -Condition (-not $forbiddenPresent) `
      -Id ($ContractId -f $FixtureStem, $forbiddenArtifact) `
      -FailureMessage ($FailureMessage -f $forbiddenArtifact, $FixtureStem) `
      -PassMessage ($PassMessage -f $forbiddenArtifact, $FixtureStem)
  }
}

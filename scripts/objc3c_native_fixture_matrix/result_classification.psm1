$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest
if ($PSVersionTable.PSVersion.Major -ge 7) {
  $PSNativeCommandUseErrorActionPreference = $false
}

Import-Module (Join-Path $PSScriptRoot "paths.psm1") -Force -DisableNameChecking

function New-Objc3cNativeFixtureMatrixPositiveResult {
  param(
    [Parameter(Mandatory = $true)][string]$FixtureRel,
    [int]$ExitCode,
    [Parameter(Mandatory = $true)][string]$CaseDir,
    [Parameter(Mandatory = $true)][string]$RepoRoot
  )

  $objPath = Join-Path $CaseDir "module.obj"
  $objExists = Test-Path -LiteralPath $objPath -PathType Leaf
  $objSize = if ($objExists) { (Get-Item -LiteralPath $objPath).Length } else { 0 }

  $passed = ($ExitCode -eq 0) -and $objExists -and ($objSize -gt 0)
  $detail = if ($passed) {
    "exit=0 obj_bytes=$objSize"
  } elseif ($ExitCode -ne 0) {
    "expected exit=0 got exit=$ExitCode"
  } elseif (!$objExists) {
    "missing module.obj"
  } else {
    "empty module.obj"
  }

  return [pscustomobject]@{
    kind = "positive"
    fixture = $FixtureRel
    passed = $passed
    detail = $detail
    exit_code = $ExitCode
    out_dir = (Get-Objc3cNativeFixtureMatrixRepoRelativePath -Path $CaseDir -Root $RepoRoot)
  }
}

Export-ModuleMember -Function "New-Objc3cNativeFixtureMatrixPositiveResult"

Set-StrictMode -Version Latest
if ($PSVersionTable.PSVersion.Major -ge 7) {
  $PSNativeCommandUseErrorActionPreference = $false
}

$script:LoweringCaseModuleRoot = Split-Path -Parent $PSScriptRoot
$script:LoweringSuiteScriptRoot = Split-Path -Parent $script:LoweringCaseModuleRoot
Import-Module (Join-Path $script:LoweringSuiteScriptRoot "objc3c_lowering_regression_suite_core.psm1") -Force -DisableNameChecking

function Get-LoweringCaseArtifacts {
  param([Parameter(Mandatory = $true)][pscustomobject]$Layout)

  $run1ObjExists = Test-Path -LiteralPath $Layout.Run1ObjPath -PathType Leaf
  $run2ObjExists = Test-Path -LiteralPath $Layout.Run2ObjPath -PathType Leaf
  $run1ObjBytes = if ($run1ObjExists) { (Get-Item -LiteralPath $Layout.Run1ObjPath).Length } else { 0 }
  $run2ObjBytes = if ($run2ObjExists) { (Get-Item -LiteralPath $Layout.Run2ObjPath).Length } else { 0 }

  $ll1Bytes = Get-FileBytesOrNull -Path $Layout.Run1LlPath
  $ll2Bytes = Get-FileBytesOrNull -Path $Layout.Run2LlPath
  $llPresentRun1 = $null -ne $ll1Bytes
  $llPresentRun2 = $null -ne $ll2Bytes
  $llPresent = $llPresentRun1 -or $llPresentRun2
  if ($llPresent) {
    $llDeterministic = $llPresentRun1 -and $llPresentRun2 -and (Test-ByteArrayEqual -Left $ll1Bytes -Right $ll2Bytes)
  }
  else {
    $llDeterministic = $true
  }

  return [pscustomobject]@{
    Run1ObjExists = $run1ObjExists
    Run2ObjExists = $run2ObjExists
    Run1ObjBytes = $run1ObjBytes
    Run2ObjBytes = $run2ObjBytes
    Manifest1Bytes = Get-FileBytesOrNull -Path $Layout.Run1ManifestPath
    Manifest2Bytes = Get-FileBytesOrNull -Path $Layout.Run2ManifestPath
    Diagnostics1Bytes = Get-FileBytesOrNull -Path $Layout.Run1DiagnosticsPath
    Diagnostics2Bytes = Get-FileBytesOrNull -Path $Layout.Run2DiagnosticsPath
    Ll1Bytes = $ll1Bytes
    Ll2Bytes = $ll2Bytes
    LlPresentRun1 = $llPresentRun1
    LlPresentRun2 = $llPresentRun2
    LlPresent = $llPresent
    LlDeterministic = $llDeterministic
  }
}

Export-ModuleMember -Function "Get-LoweringCaseArtifacts"

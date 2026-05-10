. (Join-Path $PSScriptRoot "runtime_builder_native.psm1")
. (Join-Path $PSScriptRoot "runtime_builder_positive.psm1")
. (Join-Path $PSScriptRoot "runtime_builder_negative.psm1")

function Invoke-ParserExtractionAstBuilderRuntimeCases {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [Parameter(Mandatory = $true)][string]$RunDir,
    [Parameter(Mandatory = $true)][string]$BuildScriptPath,
    [Parameter(Mandatory = $true)][string]$NativeExePath,
    [Parameter(Mandatory = $true)][bool]$NativeExeExplicit,
    [Parameter(Mandatory = $true)][string]$PositiveFixturePath,
    [Parameter(Mandatory = $true)][string[]]$NegativeFixturePaths,
    [Parameter(Mandatory = $true)]$CaseResults
  )

  Assert-ParserAstBuilderNativeExecutableReady `
    -RepoRoot $RepoRoot `
    -RunDir $RunDir `
    -BuildScriptPath $BuildScriptPath `
    -NativeExePath $NativeExePath `
    -NativeExeExplicit $NativeExeExplicit

  Invoke-ParserAstBuilderPositiveRuntimeCase `
    -RepoRoot $RepoRoot `
    -RunDir $RunDir `
    -NativeExePath $NativeExePath `
    -PositiveFixturePath $PositiveFixturePath `
    -CaseResults $CaseResults

  foreach ($negativeFixturePath in $NegativeFixturePaths) {
    Invoke-ParserAstBuilderNegativeRuntimeCase `
      -RepoRoot $RepoRoot `
      -RunDir $RunDir `
      -NativeExePath $NativeExePath `
      -NegativeFixturePath $negativeFixturePath `
      -CaseResults $CaseResults
  }
}

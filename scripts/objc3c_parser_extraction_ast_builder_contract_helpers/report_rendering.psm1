function Add-ParserAstBuilderPositiveCaseResult {
  param(
    [Parameter(Mandatory = $true)]$CaseResults,
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [Parameter(Mandatory = $true)][string]$FixturePath,
    [Parameter(Mandatory = $true)][int]$Run1Exit,
    [Parameter(Mandatory = $true)][int]$Run2Exit,
    [Parameter(Mandatory = $true)][string]$Run1Dir,
    [Parameter(Mandatory = $true)][string]$Run2Dir,
    [Parameter(Mandatory = $true)]$ArtifactDigests
  )

  $CaseResults.Add([pscustomobject]@{
      kind = "positive"
      fixture = Get-RepoRelativePath -Path $FixturePath -Root $RepoRoot
      run1_exit = $Run1Exit
      run2_exit = $Run2Exit
      run1_dir = Get-RepoRelativePath -Path $Run1Dir -Root $RepoRoot
      run2_dir = Get-RepoRelativePath -Path $Run2Dir -Root $RepoRoot
      artifact_digests = $ArtifactDigests
    }) | Out-Null
}

function Add-ParserAstBuilderNegativeCaseResult {
  param(
    [Parameter(Mandatory = $true)]$CaseResults,
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [Parameter(Mandatory = $true)][string]$FixturePath,
    [Parameter(Mandatory = $true)][string[]]$ExpectedCodes,
    [Parameter(Mandatory = $true)][int]$Run1Exit,
    [Parameter(Mandatory = $true)][int]$Run2Exit,
    [Parameter(Mandatory = $true)][string]$Run1Dir,
    [Parameter(Mandatory = $true)][string]$Run2Dir,
    [Parameter(Mandatory = $true)][string]$DiagnosticsSha256
  )

  $CaseResults.Add([pscustomobject]@{
      kind = "negative"
      fixture = Get-RepoRelativePath -Path $FixturePath -Root $RepoRoot
      expected_codes = $ExpectedCodes
      run1_exit = $Run1Exit
      run2_exit = $Run2Exit
      run1_dir = Get-RepoRelativePath -Path $Run1Dir -Root $RepoRoot
      run2_dir = Get-RepoRelativePath -Path $Run2Dir -Root $RepoRoot
      diagnostics_sha256 = $DiagnosticsSha256
    }) | Out-Null
}

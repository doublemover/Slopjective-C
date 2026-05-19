$script:ParserAstBuilderContractChecks = $null
$script:ParserAstBuilderContractRepoRoot = $null

function Set-ParserExtractionAstBuilderContractContext {
  param(
    [Parameter(Mandatory = $true)]$Checks,
    [Parameter(Mandatory = $true)][string]$RepoRoot
  )

  $script:ParserAstBuilderContractChecks = $Checks
  $script:ParserAstBuilderContractRepoRoot = $RepoRoot
}

function Assert-ParserAstBuilderContractRepoRootInitialized {
  if ([string]::IsNullOrWhiteSpace($script:ParserAstBuilderContractRepoRoot)) {
    throw "parser extraction contract FAIL: repo root has not been initialized"
  }
}

function Assert-ParserAstBuilderContractCheckSinkInitialized {
  if ($null -eq $script:ParserAstBuilderContractChecks) {
    throw "parser extraction contract FAIL: check sink has not been initialized"
  }
}

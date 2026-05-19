Set-StrictMode -Version Latest

Import-Module (Join-Path (Split-Path $PSScriptRoot -Parent) "objc3c_parser_extraction_ast_builder_contract_helpers.psm1") -Force -DisableNameChecking

. (Join-Path $PSScriptRoot "config.psm1")
. (Join-Path $PSScriptRoot "source_assertions.psm1")
. (Join-Path $PSScriptRoot "summary.psm1")

function Invoke-ParserExtractionAstBuilderContract {
  $config = New-ParserExtractionAstBuilderContractConfig
  $checks = New-Object 'System.Collections.Generic.List[object]'
  $caseResults = New-Object 'System.Collections.Generic.List[object]'
  $hadFatalError = $false
  $fatalErrorMessage = ""

  Set-ParserExtractionAstBuilderContractContext -Checks $checks -RepoRoot $config.repoRoot
  New-Item -ItemType Directory -Force -Path $config.runDir | Out-Null

  Push-Location $config.repoRoot
  try {
    Assert-ParserAstBuilderContractFiles -Config $config
    Invoke-ParserAstBuilderSourceContractAssertions -Config $config
    Invoke-ParserExtractionAstBuilderRuntimeCases `
      -RepoRoot $config.repoRoot `
      -RunDir $config.runDir `
      -BuildScriptPath $config.buildScriptPath `
      -NativeExePath $config.nativeExePath `
      -NativeExeExplicit $config.nativeExeExplicit `
      -PositiveFixturePath $config.positiveFixturePath `
      -NegativeFixturePaths $config.negativeFixturePaths `
      -CaseResults $caseResults
  }
  catch {
    $hadFatalError = $true
    $fatalErrorMessage = $_.Exception.Message
    Write-Output ("error: {0}" -f $fatalErrorMessage)
  }
  finally {
    Pop-Location
  }

  $summary = New-ParserAstBuilderContractSummary `
    -Config $config `
    -Checks $checks.ToArray() `
    -CaseResults $caseResults.ToArray() `
    -HadFatalError $hadFatalError `
    -FatalErrorMessage $fatalErrorMessage

  Write-ParserAstBuilderContractSummary -Config $config -Summary $summary

  if ($summary.status -ne "PASS") {
    return 1
  }
  return 0
}

Export-ModuleMember -Function Invoke-ParserExtractionAstBuilderContract

$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest
if ($PSVersionTable.PSVersion.Major -ge 7) {
  $PSNativeCommandUseErrorActionPreference = $false
}

Import-Module (Join-Path $PSScriptRoot "../objc3c_lexer_extraction_token_contract_helpers.psm1") -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "catalog.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "source_assertions.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "runtime_cases.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "report_renderer.psm1") -Force -DisableNameChecking

function Invoke-Objc3cLexerExtractionTokenContract {
  param([Parameter(Mandatory = $true)][string]$ScriptRoot)

  $config = New-Objc3cLexerExtractionTokenContractConfig -ScriptRoot $ScriptRoot
  $checks = New-Object 'System.Collections.Generic.List[object]'
  $caseResults = New-Object 'System.Collections.Generic.List[object]'
  $hadFatalError = $false
  $fatalErrorMessage = ""

  Set-Objc3cLexerExtractionTokenContractContext -Checks $checks -RepoRoot $config.RepoRoot

  New-Item -ItemType Directory -Force -Path $config.RunDir | Out-Null

  Push-Location $config.RepoRoot
  try {
    Invoke-Objc3cLexerExtractionTokenContractSourceAssertions -Config $config
    Invoke-Objc3cLexerExtractionTokenContractRuntimeCases -Config $config -CaseResults $caseResults
  }
  catch {
    $hadFatalError = $true
    $fatalErrorMessage = $_.Exception.Message
    Write-Output ("error: {0}" -f $fatalErrorMessage)
  }
  finally {
    Pop-Location
  }

  Write-Objc3cLexerExtractionTokenContractSummary `
    -Config $config `
    -Checks $checks `
    -CaseResults $caseResults `
    -HadFatalError $hadFatalError `
    -FatalErrorMessage $fatalErrorMessage
}

Export-ModuleMember -Function "Invoke-Objc3cLexerExtractionTokenContract"

$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest
if ($PSVersionTable.PSVersion.Major -ge 7) {
  $PSNativeCommandUseErrorActionPreference = $false
}

$runnerModuleRoot = Join-Path $PSScriptRoot "objc3c_lexer_extraction_token_contract_runner"
Import-Module (Join-Path $PSScriptRoot "objc3c_lexer_extraction_token_contract_helpers.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $runnerModuleRoot "orchestration.psm1") -Force -DisableNameChecking

Export-ModuleMember -Function "Invoke-Objc3cLexerExtractionTokenContract"

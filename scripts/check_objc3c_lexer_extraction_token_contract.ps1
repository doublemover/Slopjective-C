$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest
if ($PSVersionTable.PSVersion.Major -ge 7) {
  $PSNativeCommandUseErrorActionPreference = $false
}

Import-Module (Join-Path $PSScriptRoot "objc3c_lexer_extraction_token_contract_runner.psm1") -Force -DisableNameChecking

Invoke-Objc3cLexerExtractionTokenContract -ScriptRoot $PSScriptRoot

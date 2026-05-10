$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest
if ($PSVersionTable.PSVersion.Major -ge 7) {
  $PSNativeCommandUseErrorActionPreference = $false
}

Import-Module (Join-Path $PSScriptRoot "objc3c_parser_extraction_ast_builder_contract/runner.psm1") -Force -DisableNameChecking

exit (Invoke-ParserExtractionAstBuilderContract)

Set-StrictMode -Version Latest

$helperRoot = Join-Path $PSScriptRoot "objc3c_parser_extraction_ast_builder_contract_helpers"
$helperModules = @(
  "state.psm1",
  "fixture_paths.psm1",
  "assertions.psm1",
  "ast_artifacts.psm1",
  "report_rendering.psm1",
  "runtime_builder_cases.psm1",
  "public_surface.psm1"
)

foreach ($helperModule in $helperModules) {
  . (Join-Path $helperRoot $helperModule)
}

Export-ModuleMember -Function (Get-ParserExtractionAstBuilderContractPublicFunctions)

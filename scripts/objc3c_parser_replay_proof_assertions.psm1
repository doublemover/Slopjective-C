Set-StrictMode -Version Latest

Import-Module (Join-Path $PSScriptRoot "objc3c_parser_replay_proof_invocation.psm1") -Force -DisableNameChecking

$parserReplayProofAssertionRoot = Join-Path $PSScriptRoot "objc3c_parser_replay_proof_assertions"
$parserReplayProofAssertionModules = @(
  "data_helpers.psm1",
  "diagnostic_expectations.psm1",
  "assertion_categories.psm1",
  "fixture_case.psm1"
)

foreach ($parserReplayProofAssertionModule in $parserReplayProofAssertionModules) {
  . (Join-Path $parserReplayProofAssertionRoot $parserReplayProofAssertionModule)
}

Export-ModuleMember -Function @(
  "Invoke-Objc3cParserReplayFixtureCase"
)

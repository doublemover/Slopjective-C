$ErrorActionPreference = "Stop"

$expectationRecordsModule = Join-Path $PSScriptRoot "expectation_records.psm1"
$foundationCasesModule = Join-Path $PSScriptRoot "canonical_case_foundation.psm1"
$nilReceiverCasesModule = Join-Path $PSScriptRoot "canonical_case_nil_receiver.psm1"
$nonzeroFastPathCasesModule = Join-Path $PSScriptRoot "canonical_case_nonzero_fast_path.psm1"
$dispatchRemainderCasesModule = Join-Path $PSScriptRoot "canonical_case_dispatch_remainder.psm1"
foreach ($modulePath in @(
    $expectationRecordsModule,
    $foundationCasesModule,
    $nilReceiverCasesModule,
    $nonzeroFastPathCasesModule,
    $dispatchRemainderCasesModule
  )) {
  if (!(Test-Path -LiteralPath $modulePath -PathType Leaf)) {
    Write-Error "native recovery canonical case dependency module missing at $modulePath"
    exit 2
  }
  Import-Module $modulePath -Force -DisableNameChecking
}

function Get-CoreRecoveryCanonicalCaseDefinitionRows {
  return @(
    Get-CoreRecoveryFoundationCaseDefinitions
    Get-CoreRecoveryNilReceiverCaseDefinitions
    Get-CoreRecoveryNonzeroFastPathCaseDefinitions
    Get-CoreRecoveryDispatchRemainderCaseDefinitions
  )
}

function Get-CoreRecoveryCanonicalCaseDefinitions {
  return @(Get-CoreRecoveryCanonicalCaseDefinitionRows | ForEach-Object {
    $caseDefinition = $_
    New-RecoveryContractExpectationRecord @caseDefinition
  })
}

Export-ModuleMember -Function @(
  "Get-CoreRecoveryCanonicalCaseDefinitions"
)

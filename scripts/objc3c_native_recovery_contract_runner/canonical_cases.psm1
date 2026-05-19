$ErrorActionPreference = "Stop"

Import-Module (Join-Path $PSScriptRoot "canonical_case_definitions.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "case_invocation.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "context.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "path_config_helpers.psm1") -Force -DisableNameChecking

function Get-CoreRecoveryContractCases {
  return @(Get-CoreRecoveryCanonicalCaseDefinitions)
}

function Invoke-CoreRecoveryContractCaseRecord {
  param([object]$CaseRecord)

  if ($CaseRecord -is [System.Collections.IDictionary]) {
    Invoke-ContractCase @CaseRecord
    return
  }

  if ($CaseRecord -is [System.Collections.IEnumerable] -and -not ($CaseRecord -is [string])) {
    foreach ($entry in $CaseRecord) {
      Invoke-CoreRecoveryContractCaseRecord -CaseRecord $entry
    }
    return
  }

  throw "contract FAIL: unsupported recovery contract case record shape: $($CaseRecord.GetType().FullName)"
}

function Invoke-CoreRecoveryContractCases {
  foreach ($case in Get-CoreRecoveryContractCases) {
    Invoke-CoreRecoveryContractCaseRecord -CaseRecord $case
  }
}

function Invoke-InvalidDispatchSymbolContract {
  param(
    [string]$OutDir,
    [string]$HelloObjc3Source
  )

  $invalidDispatchConfig = New-InvalidDispatchContractConfig `
    -OutDir $OutDir `
    -HelloObjc3Source $HelloObjc3Source
  $invalidDispatchExit = Invoke-Objc3cNativeWithRecovery -Arguments @(
    $invalidDispatchConfig.Source,
    "--out-dir",
    $invalidDispatchConfig.OutDir,
    "--emit-prefix",
    "module",
    "--objc3-runtime-dispatch-symbol",
    "9invalid_symbol"
  )
  if ($invalidDispatchExit -ne 2) {
    throw "contract FAIL: invalid runtime dispatch symbol should fail with exit 2 (got $invalidDispatchExit)"
  }
  Write-Output "objc3_invalid_dispatch_symbol_rejected=true"
  Write-Output "objc3_invalid_dispatch_symbol_exit_code=$invalidDispatchExit"
}

Export-ModuleMember -Function @(
  "Get-CoreRecoveryContractCases",
  "Invoke-CoreRecoveryContractCaseRecord",
  "Invoke-CoreRecoveryContractCases",
  "Invoke-InvalidDispatchSymbolContract"
)

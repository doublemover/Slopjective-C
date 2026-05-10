Set-StrictMode -Version Latest

Import-Module (Join-Path $PSScriptRoot "state.psm1") -Force -DisableNameChecking

function Add-MissingConformanceIdFailure {
  param(
    [Parameter(Mandatory = $true)][hashtable]$State,
    [Parameter(Mandatory = $true)][System.Collections.Generic.HashSet[string]]$IdSet,
    [Parameter(Mandatory = $true)][string]$Id
  )

  if (-not $IdSet.Contains($Id)) {
    Add-ConformanceSuiteFailure -State $State -Message "Missing required conformance ID: $Id"
  }
}

function Add-MissingConformanceRangeFailures {
  param(
    [Parameter(Mandatory = $true)][hashtable]$State,
    [Parameter(Mandatory = $true)][System.Collections.Generic.HashSet[string]]$IdSet,
    [Parameter(Mandatory = $true)][string]$Prefix,
    [Parameter(Mandatory = $true)][int]$Start,
    [Parameter(Mandatory = $true)][int]$End,
    [int]$Width = 2
  )

  for ($n = $Start; $n -le $End; $n++) {
    $suffix = $n.ToString("D$Width")
    Add-MissingConformanceIdFailure -State $State -IdSet $IdSet -Id "$Prefix$suffix"
  }
}

function Invoke-ConformanceFamilyCoverageCheck {
  param([Parameter(Mandatory = $true)][hashtable]$State)

  $idSet = [System.Collections.Generic.HashSet[string]]::new([System.StringComparer]::OrdinalIgnoreCase)
  foreach ($id in $State["AllFixtureIds"]) {
    $idSet.Add($id) | Out-Null
  }

  Write-Output "Required family coverage check:"

  # Core conformance families from Part 12 / workpack #106.
  Add-MissingConformanceRangeFailures -State $State -IdSet $idSet -Prefix "TUV-" -Start 1 -End 5
  Add-MissingConformanceRangeFailures -State $State -IdSet $idSet -Prefix "CRPT-" -Start 1 -End 6
  Add-MissingConformanceRangeFailures -State $State -IdSet $idSet -Prefix "SCM-" -Start 1 -End 6
  Add-MissingConformanceRangeFailures -State $State -IdSet $idSet -Prefix "EXE-" -Start 1 -End 5
  Add-MissingConformanceRangeFailures -State $State -IdSet $idSet -Prefix "CAN-" -Start 1 -End 7
  Add-MissingConformanceRangeFailures -State $State -IdSet $idSet -Prefix "ACT-" -Start 1 -End 9
  Add-MissingConformanceRangeFailures -State $State -IdSet $idSet -Prefix "SND-" -Start 1 -End 8
  Add-MissingConformanceRangeFailures -State $State -IdSet $idSet -Prefix "SND-XM-" -Start 1 -End 2
  Add-MissingConformanceRangeFailures -State $State -IdSet $idSet -Prefix "BRW-NEG-" -Start 1 -End 5
  Add-MissingConformanceRangeFailures -State $State -IdSet $idSet -Prefix "BRW-POS-" -Start 1 -End 4
  Add-MissingConformanceRangeFailures -State $State -IdSet $idSet -Prefix "PERF-DIRMEM-" -Start 1 -End 4
  Add-MissingConformanceRangeFailures -State $State -IdSet $idSet -Prefix "PERF-DYN-" -Start 1 -End 4

  # Remaining E.3.11 / E.3.12 families.
  Add-MissingConformanceRangeFailures -State $State -IdSet $idSet -Prefix "INT-CXX-" -Start 1 -End 8
  Add-MissingConformanceRangeFailures -State $State -IdSet $idSet -Prefix "INT-SWIFT-" -Start 1 -End 8
  Add-MissingConformanceRangeFailures -State $State -IdSet $idSet -Prefix "DIAG-GRP-" -Start 1 -End 10
  Add-MissingConformanceRangeFailures -State $State -IdSet $idSet -Prefix "MIG-" -Start 1 -End 8

  # Strict-system gate families for issue #107.
  Add-MissingConformanceIdFailure -State $State -IdSet $idSet -Id "AGR-NEG-01"
  Add-MissingConformanceRangeFailures -State $State -IdSet $idSet -Prefix "AGR-RT-" -Start 1 -End 3
  Add-MissingConformanceRangeFailures -State $State -IdSet $idSet -Prefix "RES-" -Start 1 -End 6
  Add-MissingConformanceRangeFailures -State $State -IdSet $idSet -Prefix "SYS-DIAG-" -Start 1 -End 8
}

Export-ModuleMember -Function "Invoke-ConformanceFamilyCoverageCheck"

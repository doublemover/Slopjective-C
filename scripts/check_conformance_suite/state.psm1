Set-StrictMode -Version Latest

function New-ConformanceSuiteState {
  return @{
    Failures = [System.Collections.Generic.List[string]]::new()
    AllFixtureIds = [System.Collections.Generic.List[string]]::new()
    MetadataOnlyFixtureCount = 0
    ExecutableSmokeCandidates = [System.Collections.Generic.List[object]]::new()
    SmokeBucketSelection = [System.Collections.Generic.HashSet[string]]::new([System.StringComparer]::OrdinalIgnoreCase)
  }
}

function Add-ConformanceSuiteFailure {
  param(
    [Parameter(Mandatory = $true)][hashtable]$State,
    [Parameter(Mandatory = $true)][string]$Message
  )

  $State["Failures"].Add($Message) | Out-Null
}

Export-ModuleMember -Function @(
  "Add-ConformanceSuiteFailure",
  "New-ConformanceSuiteState"
)

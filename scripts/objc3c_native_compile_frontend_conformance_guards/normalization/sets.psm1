$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function New-FrontendConformanceStringSet {
  param(
    [object[]]$Values
  )

  $set = @{}
  foreach ($value in @($Values)) {
    $text = [string]$value
    if (-not [string]::IsNullOrWhiteSpace($text)) {
      $set[$text] = $true
    }
  }
  return $set
}

function New-FrontendConformanceExpectedProfileSet {
  $config = Get-FrontendConformanceInvocationConfig
  $expectedProfileSet = @{}
  foreach ($cacheMode in @($config.cache_modes)) {
    foreach ($backendMode in @($config.backend_modes)) {
      foreach ($summaryMode in @($config.summary_modes)) {
        $profileKey = "{0}|{1}|manual|{2}" -f $cacheMode, $backendMode, $summaryMode
        $expectedProfileSet[$profileKey] = $true
      }
      $profileKey = "{0}|{1}|capability-route|present" -f $cacheMode, $backendMode
      $expectedProfileSet[$profileKey] = $true
    }
  }
  return $expectedProfileSet
}

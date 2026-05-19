$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function New-FrontendFeatureStringSet {
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

function Normalize-FrontendFeatureBackendKey {
  param(
    [string]$Value
  )

  if ([string]::IsNullOrWhiteSpace($Value)) {
    return ""
  }
  return $Value.Trim().ToLowerInvariant().Replace("_", "-")
}

function New-FrontendFeatureBackendSet {
  param(
    [object[]]$Values
  )

  $set = @{}
  foreach ($value in @($Values)) {
    $backendText = ([string]$value).Trim().ToLowerInvariant()
    if (-not [string]::IsNullOrWhiteSpace($backendText)) {
      $set[$backendText] = [string]$value
    }
  }
  return $set
}

function New-FrontendFeatureLowercaseStringSet {
  param(
    [object[]]$Values
  )

  $set = @{}
  foreach ($value in @($Values)) {
    $text = ([string]$value).Trim().ToLowerInvariant()
    if (-not [string]::IsNullOrWhiteSpace($text)) {
      $set[$text] = $true
    }
  }
  return $set
}

function New-FrontendFeatureCountedFlagSet {
  param(
    [object[]]$Values
  )

  $set = @{}
  foreach ($value in @($Values)) {
    $text = [string]$value
    if (-not [string]::IsNullOrWhiteSpace($text)) {
      $set[$text] = 0
    }
  }
  return $set
}

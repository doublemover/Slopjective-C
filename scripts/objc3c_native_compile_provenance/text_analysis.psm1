$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function Get-RegexMatchCount {
  param(
    [string]$Text,
    [string]$Pattern,
    [System.Text.RegularExpressions.RegexOptions]$Options = [System.Text.RegularExpressions.RegexOptions]::Multiline
  )

  if ($null -eq $Text) {
    return 0
  }
  return ([regex]::Matches($Text, $Pattern, $Options)).Count
}

function Get-ReplayKeyCounter {
  param(
    [string]$ReplayKey,
    [string]$CounterName
  )

  if ([string]::IsNullOrWhiteSpace($ReplayKey) -or [string]::IsNullOrWhiteSpace($CounterName)) {
    return 0
  }

  $match = [regex]::Match($ReplayKey, ([regex]::Escape($CounterName) + "=([0-9]+)"))
  if (-not $match.Success) {
    return 0
  }
  return [int]$match.Groups[1].Value
}

Export-ModuleMember -Function @(
  "Get-RegexMatchCount",
  "Get-ReplayKeyCounter"
)

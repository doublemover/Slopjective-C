Set-StrictMode -Version Latest

Import-Module (Join-Path $PSScriptRoot "paths.psm1") -Force -DisableNameChecking

function Get-MissingTokens {
  param(
    [Parameter(Mandatory = $true)][string]$Text,
    [Parameter(Mandatory = $true)][string[]]$Tokens
  )

  $missing = New-Object 'System.Collections.Generic.List[string]'
  foreach ($token in $Tokens) {
    if ([string]::IsNullOrWhiteSpace($token)) {
      continue
    }
    if ($Text.IndexOf($token, [System.StringComparison]::Ordinal) -lt 0) {
      $null = $missing.Add($token)
    }
  }
  return @($missing.ToArray())
}

function Get-CanonicalLinkDiagnosticsText {
  param(
    [Parameter(Mandatory = $true)][string]$RawText,
    [Parameter(Mandatory = $true)][string]$ObjectPath,
    [Parameter(Mandatory = $true)][string]$RepoRoot
  )

  $normalizedRaw = "$RawText"
  $normalizedRaw = $normalizedRaw -replace "`r`n", "`n"
  $normalizedRaw = $normalizedRaw -replace "`r", "`n"

  $lines = New-Object 'System.Collections.Generic.List[string]'
  $null = $lines.Add("link.input_object:$(Get-RepoRelativePath -Path $ObjectPath -Root $RepoRoot)")
  $null = $lines.Add("link.input_object_basename:$([System.IO.Path]::GetFileName($ObjectPath))")

  $unresolvedSymbols = New-Object 'System.Collections.Generic.HashSet[string]' ([System.StringComparer]::Ordinal)
  foreach ($match in [regex]::Matches($normalizedRaw, '(?im)unresolved external symbol\s+([A-Za-z_.$?@][A-Za-z0-9_.$?@]*)')) {
    $null = $unresolvedSymbols.Add($match.Groups[1].Value)
  }
  foreach ($match in [regex]::Matches($normalizedRaw, '(?im)undefined (?:reference|symbol)(?:\s+to)?\s+[^A-Za-z_.$?@]*([A-Za-z_.$?@][A-Za-z0-9_.$?@]*)')) {
    $null = $unresolvedSymbols.Add($match.Groups[1].Value)
  }
  foreach ($symbol in @($unresolvedSymbols) | Sort-Object) {
    $null = $lines.Add("link.unresolved_symbol:$symbol")
  }

  $entryPointMissing = $false
  if ($normalizedRaw -match '(?im)\bentry point\b') {
    $entryPointMissing = $true
  } elseif ($normalizedRaw -match '(?im)undefined (?:reference|symbol)(?:\s+to)?\s+[^A-Za-z_.$?@]*main\b') {
    $entryPointMissing = $true
  }
  if ($entryPointMissing) {
    $null = $lines.Add("link.entrypoint_missing")
  }

  if (-not [string]::IsNullOrWhiteSpace($normalizedRaw)) {
    $null = $lines.Add("link.raw.begin")
    $null = $lines.Add($normalizedRaw.Trim())
    $null = $lines.Add("link.raw.end")
  }

  return ($lines -join "`n")
}

Export-ModuleMember -Function @(
  "Get-CanonicalLinkDiagnosticsText",
  "Get-MissingTokens"
)

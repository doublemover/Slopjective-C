$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$script:Objc3cDiagnosticsScriptsRoot = Split-Path -Parent $PSScriptRoot
Import-Module (Join-Path $script:Objc3cDiagnosticsScriptsRoot "objc3c_diagnostics_regression_suite_catalog.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "byte_arrays.psm1") -Force -DisableNameChecking

function Get-Objc3cDiagnosticsDataCore {
  param([string]$OutDir)

  $diagPath = Join-Path $OutDir "module.diagnostics.txt"
  if (-not (Test-Path -LiteralPath $diagPath -PathType Leaf)) {
    return [pscustomobject]@{
      exists = $false
      path = $diagPath
      size_bytes = 0
      text = ""
      bytes = @()
      codes = @()
      sha256 = ""
      populated = $false
    }
  }

  $bytes = [System.IO.File]::ReadAllBytes($diagPath)
  $text = [System.IO.File]::ReadAllText($diagPath)
  $sha256 = Get-Objc3cDiagnosticsByteArraySha256HexCore -Bytes $bytes

  return [pscustomobject]@{
    exists = $true
    path = $diagPath
    size_bytes = $bytes.Length
    text = $text
    bytes = $bytes
    codes = @(Get-Objc3cDiagnosticsCodesFromText -Text $text)
    sha256 = $sha256
    populated = -not [string]::IsNullOrWhiteSpace($text)
  }
}

function Get-Objc3cDiagnosticsJsonDataCore {
  param([string]$OutDir)

  $diagPath = Join-Path $OutDir "module.diagnostics.json"
  if (-not (Test-Path -LiteralPath $diagPath -PathType Leaf)) {
    return [pscustomobject]@{
      exists = $false
      path = $diagPath
      size_bytes = 0
      text = ""
      bytes = @()
      codes = @()
      sha256 = ""
      populated = $false
      valid = $false
      parse_error = ""
    }
  }

  $bytes = [System.IO.File]::ReadAllBytes($diagPath)
  $text = [System.IO.File]::ReadAllText($diagPath)
  $sha256 = Get-Objc3cDiagnosticsByteArraySha256HexCore -Bytes $bytes
  $populated = -not [string]::IsNullOrWhiteSpace($text)
  if (-not $populated) {
    return [pscustomobject]@{
      exists = $true
      path = $diagPath
      size_bytes = $bytes.Length
      text = $text
      bytes = $bytes
      codes = @()
      sha256 = $sha256
      populated = $false
      valid = $false
      parse_error = "empty diagnostics json"
    }
  }

  try {
    $payload = $text | ConvertFrom-Json
  } catch {
    return [pscustomobject]@{
      exists = $true
      path = $diagPath
      size_bytes = $bytes.Length
      text = $text
      bytes = $bytes
      codes = @()
      sha256 = $sha256
      populated = $true
      valid = $false
      parse_error = $_.Exception.Message
    }
  }

  $diagRows = @()
  if ($null -ne $payload -and $payload.PSObject.Properties.Name -contains "diagnostics") {
    $diagRows = @($payload.diagnostics)
  }
  if ($diagRows.Count -eq 0) {
    return [pscustomobject]@{
      exists = $true
      path = $diagPath
      size_bytes = $bytes.Length
      text = $text
      bytes = $bytes
      codes = @()
      sha256 = $sha256
      populated = $true
      valid = $false
      parse_error = "missing or empty diagnostics array"
    }
  }

  $uniqueCodes = New-Object 'System.Collections.Generic.List[string]'
  foreach ($row in $diagRows) {
    if ($null -eq $row) {
      continue
    }
    $code = ""
    if ($row.PSObject.Properties.Name -contains "code" -and $null -ne $row.code) {
      $code = [string]$row.code
    }
    if ([string]::IsNullOrWhiteSpace($code)) {
      continue
    }
    $normalized = $code.Trim().ToUpperInvariant()
    if (-not $uniqueCodes.Contains($normalized)) {
      $null = $uniqueCodes.Add($normalized)
    }
  }

  return [pscustomobject]@{
    exists = $true
    path = $diagPath
    size_bytes = $bytes.Length
    text = $text
    bytes = $bytes
    codes = @($uniqueCodes)
    sha256 = $sha256
    populated = $true
    valid = $true
    parse_error = ""
  }
}

Export-ModuleMember -Function @(
  "Get-Objc3cDiagnosticsDataCore",
  "Get-Objc3cDiagnosticsJsonDataCore"
)

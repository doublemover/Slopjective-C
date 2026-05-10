$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

Import-Module (Join-Path $PSScriptRoot "objc3c_diagnostics_regression_suite_catalog.psm1") -Force -DisableNameChecking

function Get-Objc3cDiagnosticsByteArraySha256Hex {
  param([byte[]]$Bytes)

  if ($null -eq $Bytes) {
    return ""
  }

  $sha256 = [System.Security.Cryptography.SHA256]::Create()
  try {
    $hashBytes = $sha256.ComputeHash($Bytes)
    return ([System.BitConverter]::ToString($hashBytes)).Replace("-", "").ToLowerInvariant()
  } finally {
    $sha256.Dispose()
  }
}

function Get-Objc3cDiagnosticsData {
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
  $sha256 = Get-Objc3cDiagnosticsByteArraySha256Hex -Bytes $bytes

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

function Get-Objc3cDiagnosticsJsonData {
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
  $sha256 = Get-Objc3cDiagnosticsByteArraySha256Hex -Bytes $bytes
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

function Test-Objc3cDiagnosticsByteArrayEqual {
  param(
    [byte[]]$Left,
    [byte[]]$Right
  )

  if ($null -eq $Left -or $null -eq $Right) {
    return $false
  }
  if ($Left.Length -ne $Right.Length) {
    return $false
  }
  for ($i = 0; $i -lt $Left.Length; $i++) {
    if ($Left[$i] -ne $Right[$i]) {
      return $false
    }
  }
  return $true
}

function Test-Objc3cDiagnosticsCodeSetExactMatch {
  param(
    [string[]]$Expected,
    [string[]]$Actual
  )

  $missing = @($Expected | Where-Object { $_ -notin $Actual })
  $unexpected = @($Actual | Where-Object { $_ -notin $Expected })
  return [pscustomobject]@{
    match = ($missing.Count -eq 0 -and $unexpected.Count -eq 0)
    missing = $missing
    unexpected = $unexpected
  }
}

function Get-Objc3cDiagnosticsUnexpectedFailClosedArtifacts {
  param(
    [string]$OutDir,
    [string[]]$ArtifactNames = @("module.manifest.json", "module.ll", "module.obj")
  )

  $unexpected = New-Object 'System.Collections.Generic.List[string]'
  foreach ($artifact in $ArtifactNames) {
    if (Test-Path -LiteralPath (Join-Path $OutDir $artifact) -PathType Leaf) {
      $null = $unexpected.Add($artifact)
    }
  }

  return @($unexpected.ToArray())
}

function Test-Objc3cDiagnosticsExpectedCodesForRun {
  param(
    [pscustomobject]$ExpectedSpec,
    [pscustomobject]$Diagnostics,
    [string]$RunLabel
  )

  $errors = New-Object 'System.Collections.Generic.List[string]'
  $codesMatch = $true

  if (@($ExpectedSpec.codes).Count -gt 0) {
    if ($Diagnostics.exists -and $Diagnostics.populated) {
      $codeCheck = Test-Objc3cDiagnosticsCodeSetExactMatch -Expected $ExpectedSpec.codes -Actual $Diagnostics.codes
      $codesMatch = $codeCheck.match
      if (-not $codeCheck.match) {
        if (@($codeCheck.missing).Count -gt 0) {
          $null = $errors.Add("$RunLabel missing expected code(s): $($codeCheck.missing -join ', ')")
        }
        if (@($codeCheck.unexpected).Count -gt 0) {
          $null = $errors.Add("$RunLabel unexpected code(s): $($codeCheck.unexpected -join ', ')")
        }
      }
    } else {
      $codesMatch = $false
      $null = $errors.Add("$RunLabel cannot enforce expected diagnostic codes because diagnostics are unavailable")
    }
  }

  return [pscustomobject]@{
    codes_match = $codesMatch
    errors = @($errors)
  }
}

Export-ModuleMember -Function @(
  "Get-Objc3cDiagnosticsByteArraySha256Hex",
  "Get-Objc3cDiagnosticsData",
  "Get-Objc3cDiagnosticsJsonData",
  "Test-Objc3cDiagnosticsByteArrayEqual",
  "Test-Objc3cDiagnosticsCodeSetExactMatch",
  "Get-Objc3cDiagnosticsUnexpectedFailClosedArtifacts",
  "Test-Objc3cDiagnosticsExpectedCodesForRun"
)

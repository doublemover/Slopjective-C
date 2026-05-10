$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$script:Objc3cDiagnosticsHeaderPattern = '(?mi)^\s*//\s*Expected diagnostic code\(s\):\s*(.+?)\s*$'
$script:Objc3cDiagnosticsCodePattern = 'O3[A-Z]\d{3}'

function Get-Objc3cDiagnosticsRepoRelativePath {
  param(
    [string]$Path,
    [string]$Root
  )

  $fullRoot = (Resolve-Path -LiteralPath $Root).Path
  $fullPath = [System.IO.Path]::GetFullPath($Path)
  if ($fullPath.StartsWith($fullRoot, [System.StringComparison]::OrdinalIgnoreCase)) {
    return $fullPath.Substring($fullRoot.Length).TrimStart('\', '/').Replace('\', '/')
  }
  return $fullPath.Replace('\', '/')
}

function Get-Objc3cDiagnosticsShortHash {
  param([string]$Value)

  $sha1 = [System.Security.Cryptography.SHA1]::Create()
  try {
    $bytes = [System.Text.Encoding]::UTF8.GetBytes($Value)
    $hashBytes = $sha1.ComputeHash($bytes)
    return ([System.BitConverter]::ToString($hashBytes)).Replace("-", "").ToLowerInvariant().Substring(0, 10)
  } finally {
    $sha1.Dispose()
  }
}

function Get-Objc3cDiagnosticsCodesFromText {
  param([AllowNull()][string]$Text)

  $uniqueCodes = New-Object 'System.Collections.Generic.List[string]'
  if ($null -eq $Text) {
    return @($uniqueCodes)
  }

  foreach ($codeMatch in [regex]::Matches($Text, $script:Objc3cDiagnosticsCodePattern)) {
    $code = $codeMatch.Value.ToUpperInvariant()
    if (-not $uniqueCodes.Contains($code)) {
      $null = $uniqueCodes.Add($code)
    }
  }

  return @($uniqueCodes)
}

function Get-Objc3cDiagnosticsExpectedCodesFromFixture {
  param([string]$FixturePath)

  $content = Get-Content -LiteralPath $FixturePath -Raw
  $headerMatch = [regex]::Match($content, $script:Objc3cDiagnosticsHeaderPattern)
  if (-not $headerMatch.Success) {
    return [pscustomobject]@{
      header_found = $false
      header_text = ""
      codes = @()
    }
  }

  return [pscustomobject]@{
    header_found = $true
    header_text = $headerMatch.Groups[1].Value.Trim()
    codes = @(Get-Objc3cDiagnosticsCodesFromText -Text $headerMatch.Groups[1].Value)
  }
}

function Get-Objc3cDiagnosticsFixtures {
  param([string]$FixtureDir)

  if (-not (Test-Path -LiteralPath $FixtureDir -PathType Container)) {
    throw "suite FAIL: missing negative fixture directory at $FixtureDir"
  }

  $fixtures = @(
    Get-ChildItem -LiteralPath $FixtureDir -Recurse -File |
      Where-Object { $_.Extension -in @(".objc3", ".m") } |
      Sort-Object -Property FullName
  )

  if ($fixtures.Count -eq 0) {
    throw "suite FAIL: no negative fixtures found in $FixtureDir"
  }

  return @($fixtures)
}

Export-ModuleMember -Function @(
  "Get-Objc3cDiagnosticsRepoRelativePath",
  "Get-Objc3cDiagnosticsShortHash",
  "Get-Objc3cDiagnosticsCodesFromText",
  "Get-Objc3cDiagnosticsExpectedCodesFromFixture",
  "Get-Objc3cDiagnosticsFixtures"
)

$script:ExpectedHeaderPattern = "(?mi)^\s*//\s*Expected diagnostic code\(s\):\s*(.+?)\s*$"
$script:DiagnosticCodePattern = "O3[A-Z]\d{3}"
$script:ParserCodePattern = "^O3P\d{3}$"

function Get-Objc3cParserReplaySha256HexFromBytes {
  param([Parameter(Mandatory = $true)][byte[]]$Bytes)

  $sha256 = [System.Security.Cryptography.SHA256]::Create()
  try {
    $hashBytes = $sha256.ComputeHash($Bytes)
    return ([System.BitConverter]::ToString($hashBytes)).Replace("-", "").ToLowerInvariant()
  }
  finally {
    $sha256.Dispose()
  }
}

function Get-Objc3cParserReplaySha256HexFromFile {
  param([Parameter(Mandatory = $true)][string]$Path)

  $bytes = [System.IO.File]::ReadAllBytes($Path)
  return Get-Objc3cParserReplaySha256HexFromBytes -Bytes $bytes
}

function Get-Objc3cParserReplayNormalizedDiagnosticCodes {
  param([string[]]$Codes)

  if ($null -eq $Codes) {
    return @()
  }

  return @(
    $Codes |
      Where-Object { -not [string]::IsNullOrWhiteSpace($_) } |
      ForEach-Object { $_.ToUpperInvariant() } |
      Sort-Object -Unique
  )
}

function Test-Objc3cParserReplayCodeSetsEqual {
  param(
    [string[]]$Left,
    [string[]]$Right
  )

  $leftSet = @(Get-Objc3cParserReplayNormalizedDiagnosticCodes -Codes $Left)
  $rightSet = @(Get-Objc3cParserReplayNormalizedDiagnosticCodes -Codes $Right)
  if ($leftSet.Count -ne $rightSet.Count) {
    return $false
  }

  for ($i = 0; $i -lt $leftSet.Count; $i++) {
    if ($leftSet[$i] -ne $rightSet[$i]) {
      return $false
    }
  }
  return $true
}

function Get-Objc3cParserReplayDiagnosticCodesFromText {
  param([Parameter(Mandatory = $true)][string]$Text)

  $codes = [regex]::Matches($Text, $script:DiagnosticCodePattern) | ForEach-Object {
    $_.Value.ToUpperInvariant()
  }
  return Get-Objc3cParserReplayNormalizedDiagnosticCodes -Codes $codes
}

function Get-Objc3cParserReplayDiagnosticCodesFromJson {
  param([Parameter(Mandatory = $true)][string]$Path)

  $payload = Get-Content -LiteralPath $Path -Raw | ConvertFrom-Json
  if ($null -eq $payload -or $null -eq $payload.diagnostics) {
    return @()
  }
  $codes = @($payload.diagnostics | ForEach-Object { "$($_.code)" })
  return Get-Objc3cParserReplayNormalizedDiagnosticCodes -Codes $codes
}
